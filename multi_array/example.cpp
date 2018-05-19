// Copyright (c) 2018 Henri Menke. All rights reserved.

#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

#ifdef __CUDACC__
#    define DEVICE_FUNC __host__ __device__
#else
#    define DEVICE_FUNC
#endif

#if __cplusplus == 201402L
#    define CXX14_CONSTEXPR constexpr
#else
#    define CXX14_CONSTEXPR
#endif

namespace meta {

// product

template <typename T, T...>
struct product;

template <typename T, T dim0, T... dim>
struct product<T, dim0, dim...> {
    static constexpr T const value = dim0 * product<T, dim...>::value;
};

template <typename T>
struct product<T> {
    static constexpr T const value = 1;
};

// less

template <typename T, T a, T b>
struct less {
    static constexpr bool const value = a < b;
};

// pack_element

template <std::size_t index, typename T, T head, T... pack>
struct pack_element {
    static_assert(index < sizeof...(pack) + 1, "index out of bounds");
    static constexpr T const value = pack_element<index - 1, T, pack...>::value;
};

template <typename T, T head, T... pack>
struct pack_element<0, T, head, pack...> {
    static constexpr T const value = head;
};

// all

template <bool...>
struct all;

template <bool... b>
struct all<true, b...> {
    static constexpr bool const value = true && all<b...>::value;
};

template <bool... b>
struct all<false, b...> {
    static constexpr bool const value = false && all<b...>::value;
};

template <>
struct all<> {
    static constexpr bool const value = true;
};

// linearized_index

template <size_t n, size_t... N>
struct linearized_index {
    template <typename... Idx>
    DEVICE_FUNC constexpr std::size_t operator()(Idx... idx) const {
        using unpack = std::size_t[];
        return unpack{std::size_t(idx)...}[n] +
               unpack{std::size_t(N)...}[n] *
                   linearized_index<n - 1, N...>{}(idx...);
    }
};

template <size_t... N>
struct linearized_index<0, N...> {
    template <typename... Idx>
    DEVICE_FUNC constexpr std::size_t operator()(Idx... idx) const {
        using unpack = std::size_t[];
        return unpack{std::size_t(idx)...}[0];
    }
};

// check_bounds

template <size_t n, size_t... N>
struct check_bounds {
    template <typename... Idx>
    constexpr bool operator()(Idx... idx) const {
        using unpack = std::size_t[];
        return unpack{std::size_t(idx)...}[n] < unpack{std::size_t(N)...}[n]
                   ? check_bounds<n - 1, N...>{}(idx...)
                   : throw std::out_of_range("index out of bounds: " +
                                             std::to_string(n));
    }
};

template <size_t... N>
struct check_bounds<0, N...> {
    template <typename... Idx>
    constexpr bool operator()(Idx... idx) const {
        using unpack = std::size_t[];
        return unpack{std::size_t(idx)...}[0] < unpack{std::size_t(N)...}[0]
                   ? false
                   : throw std::out_of_range("index out of bounds: " +
                                             std::to_string(0));
    }
};

} // namespace meta

namespace math {

template <typename T, std::size_t... N>
class multi_array {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = pointer;

private:
    template <size_type... s>
    using size_product = meta::product<size_type, s...>;

    template <size_type a, size_type b>
    using size_less = meta::less<size_type, a, b>;

    template <size_type i, size_type... pack>
    using size_element = meta::pack_element<i, size_type, pack...>;

    // Storage
    value_type m_data[size_product<N...>::value];

public:
    DEVICE_FUNC constexpr multi_array() {}

    template <typename... U>
    DEVICE_FUNC constexpr multi_array(U... data) : m_data{value_type(data)...} {}

    DEVICE_FUNC void fill(T &&initializer) {
        for (iterator it = begin(); it != end(); ++it) {
            *it = initializer;
        }
    }

    DEVICE_FUNC void swap(multi_array &other) {
        // noexcept(std::is_nothrow_swappable<value_type>::value)
        iterator oit = other.begin();
        for (iterator it = begin(); it != end(); ++it, (void)++oit) {
            value_type tmp = std::move(*it);
            *it = std::move(*oit);
            *oit = std::move(tmp);
        }
    }

    DEVICE_FUNC constexpr iterator begin() const noexcept { return iterator(data()); }

    DEVICE_FUNC constexpr iterator end() const noexcept {
        return iterator(data() + size_product<N...>::value);
    }

    DEVICE_FUNC pointer data() noexcept { return m_data; }
    DEVICE_FUNC const_pointer data() const noexcept { return m_data; }

    template <typename... Idx>
    DEVICE_FUNC CXX14_CONSTEXPR reference operator()(Idx... idx) {
        static_assert(sizeof...(idx) == sizeof...(N), "dimension mismatch");
        static_assert(
            meta::all<std::is_convertible<Idx, size_type>::value...>::value,
            "type mismatch");
        return
#if !defined(NDEBUG) || defined(__CUDACC__)
            meta::check_bounds<sizeof...(idx) - 1, N...>{}(idx...),
#endif
            m_data[meta::linearized_index<sizeof...(idx) - 1, N...>{}(idx...)];
    }

    template <typename... Idx>
    DEVICE_FUNC constexpr value_type operator()(Idx... idx) const {
        static_assert(sizeof...(idx) == sizeof...(N), "dimension mismatch");
        static_assert(
            meta::all<std::is_convertible<Idx, size_type>::value...>::value,
            "type mismatch");
        return
#if !defined(NDEBUG) || defined(__CUDACC__)
            meta::check_bounds<sizeof...(idx) - 1, N...>{}(idx...),
#endif
            m_data[meta::linearized_index<sizeof...(idx) - 1, N...>{}(idx...)];
    }

    template <size_type... idx>
    DEVICE_FUNC CXX14_CONSTEXPR reference get() {
        static_assert(sizeof...(idx) == sizeof...(N), "dimension mismatch");
        static_assert(meta::all<size_less<idx, N>::value...>::value,
                      "index out of bounds");
        return this->operator()(idx...);
    }

    template <size_type... idx>
    DEVICE_FUNC constexpr value_type get() const {
        static_assert(sizeof...(idx) == sizeof...(N), "dimension mismatch");
        static_assert(meta::all<size_less<idx, N>::value...>::value,
                      "index out of bounds");
        return this->operator()(idx...);
    }

    template <size_type i>
    DEVICE_FUNC constexpr size_type size() const noexcept {
        static_assert(i < sizeof...(N), "index out of bounds");
        return size_element<i, N...>::value;
    }

    DEVICE_FUNC constexpr size_type size() const noexcept {
        return size_product<N...>::value;
    }

    // Maybe add these member functions
    // - at
    // - (c,r,cr)begin, (c,r,cr)end
    // - front, back (how useful are those?)
    // Maybe add these operators
    // - operator==
    // - operator!=
    // - operator<
    // - operator<=
    // - operator>
    // - operator>=
};

} // namespace math

#undef CXX14_CONSTEXPR

int main() {
    math::multi_array<double, 2, 2, 2> a = {1, 2, 3, 4, 5, 6, 7, 8};

    for (std::size_t i = 0; i < a.size<0>(); ++i) {
        for (std::size_t j = 0; j < a.size<1>(); ++j) {
            for (std::size_t k = 0; k < a.size<2>(); ++k) {
                std::cout << a(i, j, k) << '\n';
            }
        }
    }

    constexpr math::multi_array<double, 2, 2> b = { 0,0,0,1 };
    static_assert(b(1,1) == 1, "!");
    static_assert(b.get<1,1>() == 1, "!");
}
