#include "PythonCallback.hpp"

namespace conversions {

// Convert C++ type to Python

PyObject* arg_to_python(double x)
{
  return PyFloat_FromDouble(x);
}

} // namespace conversions
