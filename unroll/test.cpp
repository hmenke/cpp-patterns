#include <iostream>
#include "unroll.hpp"

int main() {
    int sum = 0;
    unroll<range<-2, 1>, range<0, 2>, range<3, 0>>(
        [&sum](int i, int j, int k) { sum += i + j + k; });
    std::cout << sum << '\n';

    sum = 0;
    for (int i = -2; i < 1; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 3; k > 0; --k) {
                sum += i + j + k;
            }
        }
    }
    std::cout << sum << '\n';
}
