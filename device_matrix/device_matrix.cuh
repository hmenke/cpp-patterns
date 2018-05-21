// Copyright (c) 2018 Henri Menke. All rights reserved.

#include <iomanip>
#include <limits>
#include <type_traits>

#include <thrust/device_vector.h>
#include <thrust/functional.h>
#include <thrust/tabulate.h>

#include <cublas_v2.h>
#include <cusolverDn.h>

namespace math {

/// \cond

namespace internal {

template <typename T>
struct cublas { using value_type = T; };

template <>
struct cublas<double> {
    static void geam(double const *A, double *C, int m, int n) {
        double const alpha = 1;
        double const beta = 0;

        cublasHandle_t handle;
        assert(CUBLAS_STATUS_SUCCESS == cublasCreate(&handle));
        assert(CUBLAS_STATUS_SUCCESS == cublasDgeam(handle, CUBLAS_OP_T,
                                                    CUBLAS_OP_T, n, m, &alpha,
                                                    A, m, &beta, A, m, C, n));
        cublasDestroy(handle);
    }

    static void gemm(const double *A, const double *B, double *C, int m, int k,
                     int n) {
        int lda = m, ldb = k, ldc = m;
        double const alpha = 1;
        double const beta = 0;

        cublasHandle_t handle;
        assert(CUBLAS_STATUS_SUCCESS == cublasCreate(&handle));

        assert(CUBLAS_STATUS_SUCCESS ==
               cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, m, n, k, &alpha, A,
                           lda, B, ldb, &beta, C, ldc));

        cublasDestroy(handle);
    }
};

template <typename T>
struct cusolver;

template <>
struct cusolver<double> {
    static void potrf(double *A, double *B, int N) {
        cusolverDnHandle_t handle;
        assert(CUSOLVER_STATUS_SUCCESS == cusolverDnCreate(&handle));

        int lwork = -1;
        assert(CUSOLVER_STATUS_SUCCESS ==
               cusolverDnDpotrf_bufferSize(handle, CUBLAS_FILL_MODE_UPPER, N, A,
                                           N, &lwork));

        assert(lwork != -1);

        thrust::device_vector<double> workspace(lwork);
        thrust::device_vector<int> info(1);
        assert(CUSOLVER_STATUS_SUCCESS ==
               cusolverDnDpotrf(handle, CUBLAS_FILL_MODE_UPPER, N, A, N,
                                thrust::raw_pointer_cast(workspace.data()),
                                lwork, thrust::raw_pointer_cast(info.data())));
        assert(info[0] == 0);

        assert(CUSOLVER_STATUS_SUCCESS ==
               cusolverDnDpotrs(handle, CUBLAS_FILL_MODE_UPPER, N, N, A, N, B,
                                N, thrust::raw_pointer_cast(info.data())));
        assert(info[0] == 0);

        cusolverDnDestroy(handle);
    }
};

template <typename T>
struct almostEqual {
    T epsilon;
    __host__ __device__ bool operator()(T x, T y) const {
        return std::abs(x - y) < epsilon;
    }
};

__host__ __device__ inline thrust::tuple<size_t, size_t>
index_to_ij(size_t index, size_t lda) {
    size_t i, j;
    i = index % lda;
    index /= lda;
    j = index;
    return thrust::make_tuple(i, j);
    ;
}

template <typename T>
struct IdentityGenerator {
    size_t lda;

    __host__ __device__ T operator()(size_t index) {
        size_t i, j;
        thrust::tie(i, j) = index_to_ij(index, lda);
        return T(i == j ? 1.0 : 0.0);
    }
};

} // namespace internal

/// \endcond

template <typename T>
class device_matrix {
private:
    using storage_type = thrust::device_vector<T>;

public:
    using value_type = typename storage_type::value_type;
    using size_type = typename storage_type::size_type;
    using difference_type = typename storage_type::difference_type;
    using reference = typename storage_type::reference;
    using const_reference = typename storage_type::const_reference;
    using pointer = typename storage_type::pointer;
    using const_pointer = typename storage_type::const_pointer;
    using iterator = typename storage_type::iterator;

private:
    // Storage
    size_type m_rows;
    size_type m_cols;
    thrust::device_vector<T> m_data;

public:
    explicit device_matrix(size_type rows, size_type cols)
        : m_rows(rows), m_cols(cols), m_data(m_rows * m_cols) {}

    reference operator()(size_type row, size_type col) noexcept {
        return m_data[row + col * m_rows];
    }

    const_reference operator()(size_type row, size_type col) const noexcept {
        return m_data[row + col * m_rows];
    }

    void fill(value_type const &value) noexcept(
        std::is_nothrow_copy_assignable<value_type>::value) {
        thrust::fill(m_data.begin(), m_data.end(), value);
    }

    void
    swap(device_matrix &other) noexcept(noexcept(m_data.swap(other.m_data))) {
        std::swap(m_rows, other.m_rows);
        std::swap(m_cols, other.m_cols);
        m_data.swap(other.m_data);
    }

    pointer data() noexcept { return m_data.data(); }
    const_pointer data() const noexcept { return m_data.data(); }
    size_type rows() const noexcept { return m_rows; }
    size_type cols() const noexcept { return m_cols; }
    size_type size() const noexcept { return m_rows * m_cols; }

    /// \defgroup arithmetic Arithmetic operators
    /// \{

    /// Matrix-matrix multiplication
    device_matrix operator*(device_matrix const &B) const {
        assert(m_cols == B.m_rows);
        device_matrix C(m_rows, B.m_cols);
        internal::cublas<value_type>::gemm(thrust::raw_pointer_cast(data()),
                                           thrust::raw_pointer_cast(B.data()),
                                           thrust::raw_pointer_cast(C.data()),
                                           m_rows, m_cols, B.m_cols);
        return C;
    }

    /// Add element-wise
    device_matrix operator+(device_matrix const &B) const {
        assert(m_rows == B.m_rows);
        assert(m_cols == B.m_cols);
        device_matrix C(m_rows, m_cols);
        thrust::transform(data(), data() + size(), B.data(), C.data(),
                          thrust::plus<value_type>{});
        return C;
    }

    /// Subtract element-wise
    device_matrix operator-(device_matrix const &B) const {
        assert(m_rows == B.m_rows);
        assert(m_cols == B.m_cols);
        device_matrix C(m_rows, m_cols);
        thrust::transform(data(), data() + size(), B.data(), C.data(),
                          thrust::minus<value_type>{});
        return C;
    }

    /// Negate element-wise
    device_matrix &operator-() {
        thrust::transform(data(), data() + size(), data(),
                          thrust::negate<value_type>{});
        return *this;
    }

    /// \}

    /// \defgroup compare Comparison
    /// \{

    /// Compare for exact bit-wise equality
    bool operator==(device_matrix const &B) const {
        return m_rows == B.m_rows && m_cols == B.m_cols &&
               thrust::equal(data(), data() + size(), B.data());
    }

    /// Compare for equality with threshold \p epsilon.
    ///
    /// \param epsilon maximum allowed difference
    bool almostEqual(device_matrix const &B,
                     value_type const epsilon =
                         std::numeric_limits<value_type>::epsilon()) const {
        return m_rows == B.m_rows && m_cols == B.m_cols &&
               thrust::equal(data(), data() + size(), B.data(),
                             internal::almostEqual<value_type>{epsilon});
    }

    /// \}

    /// \defgroup linalg Linear algebra
    /// \{

    /// Compute the transpose.
    device_matrix transpose() const {
        device_matrix C(m_cols, m_rows);
        internal::cublas<value_type>::geam(thrust::raw_pointer_cast(  data()),
                                           thrust::raw_pointer_cast(C.data()),
                                           m_rows, m_cols);
        return C;
    }

    /// Compute the inverse.
    device_matrix inverse() const {
        assert(m_rows == m_cols);
        device_matrix A = *this;
        device_matrix B = device_matrix::Identity(m_rows, m_cols);

        internal::cusolver<double>::potrf(thrust::raw_pointer_cast(A.data()),
                                          thrust::raw_pointer_cast(B.data()),
                                          m_rows);

        return B;
    }

    /// \}

    /// \defgroup generators Convenience generator functions
    /// \{

    /// Generate an identity matrix with size \p rows x \p cols.
    static device_matrix Identity(size_type rows, size_type cols) {
        device_matrix I(rows, cols);
        thrust::tabulate(I.data(), I.data() + I.size(),
                         internal::IdentityGenerator<value_type>{rows});
        return I;
    }

    /// \}
};

} // namespace math

template <typename T>
std::ostream &operator<<(std::ostream &os, math::device_matrix<T> const &m) {
    os << '{';
    for (size_t i = 0; i < m.rows(); ++i) {
        if (i > 0) {
            os << ' ';
        }
        os << '{';
        for (size_t j = 0; j < m.cols(); ++j) {
            os << std::setw(12) << m(i, j);
            if (j < m.cols() - 1) {
                os << ',';
            }
        }
        os << '}';
        if (i < m.rows() - 1) {
            os << ",\n";
        }
    }
    os << '}';
    return os;
}
