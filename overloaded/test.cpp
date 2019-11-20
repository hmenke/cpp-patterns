#include <iostream>
#include <string>
#include <tuple>

namespace cxx17 {

namespace detail {

template <typename... Args>
struct overloaded : Args... {
    using Args::operator()...;
};

template <typename... Args>
overloaded(Args...)->overloaded<Args...>;

} // namespace detail

#define overload inline constexpr cxx17::detail::overloaded

} // namespace cxx17

overload f{[](int, double, float) {
               std::cout << "cxx17::" << __PRETTY_FUNCTION__ << '\n';
           },
           [](int, double, std::string) {
               std::cout << "cxx17::" << __PRETTY_FUNCTION__ << '\n';
           }};

#undef overload // I want to use the same name for the C++11 variant

namespace cxx11 {

namespace detail {

template <typename... Args>
struct overloaded;

template <typename Arg>
struct overloaded<Arg> : Arg {
    overloaded(Arg &&arg) : Arg{std::forward<Arg>(arg)} {}
    using Arg::operator();
};

template <typename Arg, typename... Args>
struct overloaded<Arg, Args...> : Arg, overloaded<Args...> {
    overloaded(Arg &&arg, Args &&... args)
        : Arg{std::forward<Arg>(arg)}, overloaded<Args...>{
                                           std::forward<Args>(args)...} {}
    using Arg::operator();
    using overloaded<Args...>::operator();
};

} // namespace detail

template <typename... Args>
detail::overloaded<Args...> overload(Args &&... args) {
    return detail::overloaded<Args...>{std::forward<Args>(args)...};
};

} // namespace cxx11

static auto const g = cxx11::overload(
    [](int, double, float) {
        std::cout << "cxx11::" << __PRETTY_FUNCTION__ << '\n';
    },
    [](int, double, std::string) {
        std::cout << "cxx11::" << __PRETTY_FUNCTION__ << '\n';
    });

int main() {
    std::tuple<int, double, float> a{1, 3.14, 1.23f};
    std::tuple<int, double, std::string> b{1, 3.14, "Hello"};

    std::apply(f, a);
    std::apply(f, b);

    std::apply(g, a);
    std::apply(g, b);
}
