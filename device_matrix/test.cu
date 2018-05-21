#include <iostream>
#include "device_matrix.cuh"

int main() {
    math::device_matrix<double> a(3,3);

    a(0,0) =  2; a(0,1) = -1; a(0,2) =  0;
    a(1,0) = -1; a(1,1) =  2; a(1,2) = -1;
    a(2,0) =  0; a(2,1) = -1; a(2,2) =  2;

    math::device_matrix<double> i(3,3);

    i(0,0) = 3./4.; i(0,1) = 2./4.; i(0,2) = 1./4.;
    i(1,0) = 2./4.; i(1,1) = 4./4.; i(1,2) = 2./4.;
    i(2,0) = 1./4.; i(2,1) = 2./4.; i(2,2) = 3./4.;

    math::device_matrix<double> b = a.inverse();
    std::cout << std::boolalpha << b.almostEqual(i,1e-12) << '\n';
    math::device_matrix<double> c = b.inverse();
    std::cout << std::boolalpha << c.almostEqual(a,1e-12) << '\n';

    std::cout << a << '\n'
              << b << '\n'
              << i << '\n';

    std::cout << (a * math::device_matrix<double>::Identity(3,3).transpose()) << '\n';
}
