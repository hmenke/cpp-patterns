#pragma once

#include <type_traits>
#include <utility>

// Inspired by https://nilsdeppe.com/posts/for-constexpr but for C++11

/// Signed version of std::size_t
using ssize_t = std::make_signed<std::size_t>::type;

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#define FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline // :(
#endif

/// Compile-time loop bounds
template <ssize_t Lower, ssize_t Upper>
struct range {
    static constexpr const ssize_t lower = Lower;
    static constexpr const ssize_t upper = Upper;
};

/// \cond

namespace internal {

template <ssize_t... Is>
struct index_range {};

template <ssize_t L, ssize_t U, ssize_t... Is>
struct make_index_range_ascending
    : make_index_range_ascending<L, U - 1, U - 1, Is...> {};

template <ssize_t L, ssize_t... Is>
struct make_index_range_ascending<L, L, Is...> : index_range<Is...> {};

template <ssize_t U, ssize_t L, ssize_t... Is>
struct make_index_range_descending
    : make_index_range_descending<U, L + 1, L + 1, Is...> {};

template <ssize_t U, ssize_t... Is>
struct make_index_range_descending<U, U, Is...> : index_range<Is...> {};

template <ssize_t L, ssize_t U>
using make_index_range =
    typename std::conditional<(L < U), make_index_range_ascending<L, U>,
                              make_index_range_descending<L, U>>::type;

template <ssize_t... I, typename F, typename... Indices>
FORCE_INLINE constexpr int unroll_impl(F &&f, index_range<I...>,
                                              Indices... v) {
    using expander = int[];
    return expander{0, ((void)(f(v..., I)), 0)...}[0];
}

template <ssize_t... I, ssize_t Lower, ssize_t Upper, typename F,
          typename... Indices>
FORCE_INLINE constexpr int unroll_impl(F &&f, range<Lower, Upper>,
                                              index_range<I...>, Indices... v) {
    using expander = int[];
    return expander{
        0, ((void)(unroll_impl(std::forward<F>(f),
                               internal::make_index_range<Lower, Upper>{}, v...,
                               I)),
            0)...}[0];
}

template <typename Range0, typename... Range, ssize_t... I, ssize_t Lower,
          ssize_t Upper, typename F, typename... Indices>
FORCE_INLINE constexpr int unroll_impl(F &&f, range<Lower, Upper>,
                                              index_range<I...>, Indices... v) {
    using expander = int[];
    return expander{0,
                    ((void)(unroll_impl<Range...>(
                         std::forward<F>(f), Range0{},
                         internal::make_index_range<Lower, Upper>{}, v..., I)),
                     0)...}[0];
}

} // namespace internal

/// \endcond

/// Unroll loop at compile-time
template <typename Range, typename F>
FORCE_INLINE constexpr int unroll(F &&f) {
    return internal::unroll_impl(
        std::forward<F>(f),
        internal::make_index_range<Range::lower, Range::upper>{});
}

/// Unroll loop at compile-time (nested case)
template <typename Range0, typename Range1, typename... Range, typename F>
FORCE_INLINE constexpr int unroll(F &&f) {
    return internal::unroll_impl<Range...>(
        std::forward<F>(f), Range1{},
        internal::make_index_range<Range0::lower, Range0::upper>{});
}
