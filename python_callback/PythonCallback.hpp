#pragma once
#include <Python.h>

// ===========
// Conversions
// ===========

namespace conversions {

// Convert C++ type to Python

PyObject* arg_to_python(double x);

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

// Concatenate characters to string

template < char... Args >
struct do_make_signature_indeed
{
  static constexpr const char str[] = { '(', Args..., ')', '\0' };
};

template < char... Args >
constexpr const char do_make_signature_indeed<Args...>::str[];

// Recursively populate variadic argument list

template < size_t N, char Arg, char... Args >
struct do_make_signature
{
  typedef typename do_make_signature<N-1,Arg,Arg,Args...>::type type;
};

template < char Arg, char... Args >
struct do_make_signature<0,Arg,Args...>
{
  typedef do_make_signature_indeed<Arg,Args...> type;
};

// Interface: Call recursion

template < size_t N, char Arg >
struct make_signature
{
  typedef typename do_make_signature<N-1,Arg>::type type;
};

template < char Arg >
struct make_signature<0,Arg>
{
  typedef do_make_signature_indeed<> type;
};

// Alias template for easy access

template < size_t N, char Arg >
using signature = typename make_signature<N,Arg>::type;

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
