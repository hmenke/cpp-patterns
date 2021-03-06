#include <iostream>
#include "device_matrix.hpp"

struct cpu_count_elements {
    device_matrix_view<double,policy::host> v;
    void operator()(int idx) {
        std::size_t i,j;
        thrust::tie(i,j) = unravel_index(idx,v.rows());
        v(i,j) = idx;
    }
};

void test_cpu() {
    device_matrix<double,policy::host> a(3,3);

    a(0,0) =  2; a(0,1) = -1; a(0,2) =  0;
    a(1,0) = -1; a(1,1) =  2; a(1,2) = -1;
    a(2,0) =  0; a(2,1) = -1; a(2,2) =  2;

    device_matrix<double,policy::host> i(3,3);

    i(0,0) = 3./4.; i(0,1) = 2./4.; i(0,2) = 1./4.;
    i(1,0) = 2./4.; i(1,1) = 4./4.; i(1,2) = 2./4.;
    i(2,0) = 1./4.; i(2,1) = 2./4.; i(2,2) = 3./4.;

    device_matrix<double,policy::host> b = a.inverse();
    std::cout << std::boolalpha << b.almostEqual(i,1e-12) << '\n';
    device_matrix<double,policy::host> c = b.inverse();
    std::cout << std::boolalpha << c.almostEqual(a,1e-12) << '\n';

    std::cout << a << '\n'
              << b << '\n'
              << i << '\n';

    std::cout << (a * device_matrix<double,policy::host>::Identity(3,3).transpose()) << '\n';

    auto begin = thrust::make_counting_iterator(0);
    auto end = begin + a.size();
    thrust::for_each(thrust::host, begin, end, cpu_count_elements{a});

    std::cout << a << '\n';
}
