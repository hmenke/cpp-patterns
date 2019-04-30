#include <iostream>
#include <string>
#include <tuple>

namespace detail {

template <typename... Args>
struct overloaded : Args... {
    using Args::operator()...;
};

template <typename... Args>
overloaded(Args...)->overloaded<Args...>;

} // namespace detail

#define overload inline constexpr detail::overloaded

overload f {
    [](int, double, float) { std::cout << __PRETTY_FUNCTION__ << '\n'; },
    [](int, double, std::string) { std::cout << __PRETTY_FUNCTION__ << '\n'; }
};

int main() {
    std::tuple<int, double, float> a{1, 3.14, 1.23f};
    std::tuple<int, double, std::string> b{1, 3.14, "Hello"};

    std::apply(f, a);
    std::apply(f, b);
}
