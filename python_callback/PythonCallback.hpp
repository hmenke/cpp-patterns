#pragma once
#include <Python.h>

// ===========
// Conversions
// ===========

namespace conversions {

// Convert C++ type to Python

inline PyObject* arg_to_python(double x)
{
  return PyFloat_FromDouble(x);
}

// Convert Python type to C++

template < typename T >
struct return_from_python
{
  T operator() (PyObject*);
};

template < >
struct return_from_python<double>
{
  double operator() (PyObject* v) const
  {
      return PyFloat_AsDouble(v);
  }
};

} // namespace conversions

// =================
// Signature Handler
// =================

namespace internal {

template <std::size_t N, char C, char... Cs>
struct signature : signature<N - 1, C, C, Cs...> {};

template <char C, char... Cs>
struct signature<0UL, C, Cs...> {
    static constexpr char const str[] = {'(', Cs..., ')', '\0'};
};
template <char C, char... Cs>
constexpr char const signature<0UL, C, Cs...>::str[];

} // namespace internal

// ===============
// Function caller
// ===============

template < typename ReturnType, typename ... Args >
ReturnType call(PyObject* callable, Args ... args)
{
  // Acquire GIL
  PyGILState_STATE gil = PyGILState_Ensure();

  // Call the function
  PyObject* const result =
    PyEval_CallFunction(
      callable,
      internal::signature<sizeof...(args), 'O'>::str,
      conversions::arg_to_python(args)...
      );

  conversions::return_from_python<ReturnType> converter;

  // Release GIL
  PyGILState_Release(gil);

  return converter(result);
}
