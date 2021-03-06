#include <iostream>
#include <string>

template <typename T>
const char * print_type_impl() {
#if defined(__clang__)
    return __PRETTY_FUNCTION__;
#elif defined(__GNUC__) && !defined(__clang__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#else
    #error "No support for this compiler."
#endif
}

template <typename T>
struct type_tag {};

template <int&... ExplicitArgumentBarrier, typename T>
std::string print_type(type_tag<T>) {
#if defined(__clang__)
    size_t prefixlen = sizeof("const char *print_type_impl() [T = ") - 1;
    size_t suffixlen = sizeof("]") - 1;
#elif defined(__GNUC__) && !defined(__clang__)
    size_t prefixlen = sizeof("const char *print_type_impl() [T = ") - 1;
    size_t suffixlen = sizeof("]") - 1;
#elif defined(_MSC_VER)
    size_t prefixlen = sizeof("const char *__cdecl print_type_impl<") - 1;
    size_t suffixlen = sizeof(">(void)") - 1;
#else
    #error "No support for this compiler."
#endif
    std::string type = print_type_impl<T>();
    return type.substr(prefixlen, type.size() - prefixlen - suffixlen);
}

std::string print_type(type_tag<std::string>) {
    return "std::string";
}

int main() {
    int i;
    std::cout << print_type(type_tag<decltype(i)>{}) << '\n';
    std::string s;
    std::cout << print_type(type_tag<decltype(s)>{}) << '\n';
}
