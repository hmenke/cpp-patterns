#include "PythonCallback.hpp"
#include <iostream>
#include <cmath>

// Internal function which calls the callback

void cpp_function(PyObject* f)
{
  auto x = call<double>(f, M_PI);
  std::cout << "C++: " << x << "\n";
}
