#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>

class Tee {
    std::vector<std::reference_wrapper<std::ostream>> oss;

    template <typename T>
    Tee &output(T &&t) {
        for (auto &&os : oss) {
            os.get() << std::forward<T>(t);
        }
        return *this;
    }

public:
    template <typename... Args>
    Tee(Args &&... args) : oss{args...} {}

    template <typename T>
    Tee &operator<<(T &&t) {
        return output(std::forward<T>(t));
    }

    Tee &operator<<(std::ostream &(*t)(std::ostream &)) {
        return output(t);
    }
};

int main() {
    double d = 3.14;
    int i = 15;
    bool b = true;
    Tee(std::cout, std::cerr) << "Hello"
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
