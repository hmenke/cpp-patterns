#include <iostream>
#include <Eigen/Dense>
#include <Eigen/LU>

namespace cuda {

template < typename MatrixType, typename ResultType >
__device__ void inverse(MatrixType * matrix, ResultType * inverse)
{
    typedef typename MatrixType::Scalar Scalar;
    Eigen::internal::compute_inverse<MatrixType,ResultType>::run(*matrix,*inverse);
}

template < typename MatrixType >
__device__ void inverse_inplace(MatrixType * matrix)
{
    MatrixType inv;
    inverse(matrix,&inv);
    *matrix = inv;
}

}

template < typename MatrixType, typename ResultType >
__global__ void inverse_kernel(MatrixType * matrix, ResultType * inverse)
{
    cuda::inverse(matrix,inverse);
}

template < typename MatrixType >
__global__ void inverse_inplace_kernel(MatrixType * matrix)
{
    cuda::inverse_inplace(matrix);
}

int main(void)
{
    Eigen::Matrix3d mat = Eigen::Matrix3d::Random(4,4);

    // Host computaiton
    std::cout << mat.inverse() << '\n';

    // Device computaiton
    Eigen::Matrix3d inv;

    Eigen::Matrix3d * pmat;
    Eigen::Matrix3d * pinv;

    cudaMalloc(&pmat, sizeof(Eigen::Matrix3d));
    cudaMalloc(&pinv, sizeof(Eigen::Matrix3d));

    cudaMemcpy(pmat, &mat, sizeof(Eigen::Matrix3d), cudaMemcpyHostToDevice);

    inverse_kernel<<<1,1>>>(pmat,pinv);
    std::cerr << "CUDA exit status: " << cudaGetErrorString(cudaGetLastError()) << "\n";

    cudaMemcpy(&inv, pinv, sizeof(Eigen::Matrix3d), cudaMemcpyDeviceToHost);
    std::cout << inv << '\n';

    inverse_inplace_kernel<<<1,1>>>(pmat);
    std::cerr << "CUDA exit status: " << cudaGetErrorString(cudaGetLastError()) << "\n";

    cudaMemcpy(&mat, pmat, sizeof(Eigen::Matrix3d), cudaMemcpyDeviceToHost);
    std::cout << mat << '\n';
}
