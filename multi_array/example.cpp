#include <iostream>
#include "multi_array.hpp"

int main() {
    math::multi_array<double, 2, 2, 2> a = {1, 2, 3, 4, 5, 6, 7, 8};

    for (std::size_t i = 0; i < a.size<0>(); ++i) {
        for (std::size_t j = 0; j < a.size<1>(); ++j) {
            for (std::size_t k = 0; k < a.size<2>(); ++k) {
                std::cout << a(i, j, k) << '\n';
            }
        }
    }

    constexpr math::multi_array<double, 2, 2> const b = {0, 0, 0, 1};

    static_assert( b(1, 1) == 1, "!" );

    constexpr math::multi_array<double, 4, 3, 2> const c = {
        0, 0,   0, 0,   0, 0,
        0, 0,   0, 0,   0, 0,
        0, 0,   2, 0,   0, 0,
        0, 0,   0, 0,   0, 1
    };

    static_assert( c(3, 2, 1) == 1, "!" );
    static_assert( c(2, 1, 0) == 2, "!" );
}
