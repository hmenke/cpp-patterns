#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>

#if __cplusplus == 201402L
#define CXX14_CONSTEXPR constexpr
#else
#define CXX14_CONSTEXPR
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

// index_sequence

// https://stackoverflow.com/a/24481400/1944004
template <std::size_t... I>
struct index_sequence {};

template <std::size_t N, std::size_t... I>
struct make_index_sequence : public make_index_sequence<N - 1, N - 1, I...> {};

template <std::size_t... I>
struct make_index_sequence<0, I...> : public index_sequence<I...> {};

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

    template <size_type... I, typename... Idx>
    CXX14_CONSTEXPR size_type linearized_index(meta::index_sequence<I...>,
                                               Idx... idx) const {
        size_type index = 0;
        using unpack = size_type[];
        (void)unpack{0UL,
                     ((void)(index = (index + unpack{size_type(idx)...}[I]) *
                                     size_element<I + 1, N...>::value),
                      0UL)...};
        return index + unpack{size_type(idx)...}[sizeof...(idx) - 1];
    }

#ifndef NDEBUG
#define check_bounds(a, b) check_bounds_impl(a, b)
#else
#define check_bounds(a, b)
#endif // NDEBUG

    template <size_type... I, typename... Idx>
    CXX14_CONSTEXPR void check_bounds_impl(meta::index_sequence<I...>,
                                           Idx... idx) const {
        using unpack = size_type[];
        (void)unpack{
            0UL,
            ((void)(unpack{size_type(idx)...}[I] < size_element<I, N...>::value
                        ? 0
                        : throw std::out_of_range("index out of bounds: " +
                                                  std::to_string(I))),
             0UL)...};
    }

    // Storage
    value_type m_data[size_product<N...>::value];

public:
    constexpr multi_array() {}

    template <typename... U>
    constexpr multi_array(U... data) : m_data{value_type(data)...} {}

    void fill(T &&initializer) {
        for (iterator it = begin(); it != end(); ++it) {
            *it = initializer;
        }
    }

    void swap(multi_array &other) {
        // noexcept(std::is_nothrow_swappable<value_type>::value)
        iterator oit = other.begin();
        for (iterator it = begin(); it != end(); ++it, (void)++oit) {
            value_type tmp = std::move(*it);
            *it = std::move(*oit);
            *oit = std::move(tmp);
        }
    }

    constexpr iterator begin() const noexcept { return iterator(data()); }

    constexpr iterator end() const noexcept {
        return iterator(data() + size_product<N...>::value);
    }

    pointer data() noexcept { return m_data; }
    const_pointer data() const noexcept { return m_data; }

    template <typename... Idx>
    CXX14_CONSTEXPR reference operator()(Idx... idx) noexcept {
        static_assert(sizeof...(idx) == sizeof...(N), "dimension mismatch");
        static_assert(
            meta::all<std::is_convertible<Idx, size_type>::value...>::value,
            "type mismatch");
        check_bounds(meta::make_index_sequence<sizeof...(idx)>{}, idx...);
        size_type index = linearized_index(
            meta::make_index_sequence<sizeof...(idx) - 1>{}, idx...);
        return m_data[index];
    }

    template <typename... Idx>
    CXX14_CONSTEXPR value_type operator()(Idx... idx) const noexcept {
        static_assert(sizeof...(idx) == sizeof...(N), "dimension mismatch");
        static_assert(
            meta::all<std::is_convertible<Idx, size_type>::value...>::value,
            "type mismatch");
        check_bounds(meta::make_index_sequence<sizeof...(idx)>{}, idx...);
        size_type index = linearized_index(
            meta::make_index_sequence<sizeof...(idx) - 1>{}, idx...);
        return m_data[index];
    }

    template <size_type... idx>
    CXX14_CONSTEXPR reference get() noexcept {
        static_assert(sizeof...(idx) == sizeof...(N), "dimension mismatch");
        static_assert(meta::all<size_less<idx, N>::value...>::value,
                      "index out of bounds");
        check_bounds(meta::make_index_sequence<sizeof...(idx)>{}, idx...);
        size_type index = linearized_index(
            meta::make_index_sequence<sizeof...(idx) - 1>{}, idx...);
        return m_data[index];
    }

    template <size_type... idx>
    CXX14_CONSTEXPR value_type get() const noexcept {
        static_assert(sizeof...(idx) == sizeof...(N), "dimension mismatch");
        static_assert(meta::all<size_less<idx, N>::value...>::value,
                      "index out of bounds");
        check_bounds(meta::make_index_sequence<sizeof...(idx)>{}, idx...);
        size_type index = linearized_index(
            meta::make_index_sequence<sizeof...(idx) - 1>{}, idx...);
        return m_data[index];
    }

    template <size_type i>
    constexpr size_type size() const noexcept {
        static_assert(i < sizeof...(N), "index out of bounds");
        return size_element<i, N...>::value;
    }

    constexpr size_type size() const noexcept {
        return size_product<N...>::value;
    }

#undef check_bounds
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
