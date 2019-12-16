
#include <functional>
#include <iomanip>
#include <iostream>

template <size_t N>
class tee_impl {
    std::reference_wrapper<std::ostream> oss[N];

    template <typename T>
    tee_impl &output(T &&t) {
        for (auto &&os : oss) {
            os.get() << std::forward<T>(t);
        }
        return *this;
    }

public:
    template <typename... Args>
    tee_impl(Args &&... args) : oss{args...} {}

    template <typename T>
    tee_impl &operator<<(T &&t) {
        return output(std::forward<T>(t));
    }

    tee_impl &operator<<(std::ostream &(*t)(std::ostream &)) {
        return output(t);
    }
};

template <typename... Args>
tee_impl<sizeof...(Args)> tee(Args &&... args) {
    return tee_impl<sizeof...(Args)>(std::forward<Args>(args)...);
}

int main() {
    double d = 3.14;
    int i = 15;
    bool b = true;
    tee(std::cout, std::cerr) << "Hello"
                              << " world!\n"
                              //
                              << std::hex
                              << i
                              //
                              << std::setprecision(10) << std::fixed
                              << d
                              //
                              << std::boolalpha
                              << b
                              //
                              << std::endl;
}
