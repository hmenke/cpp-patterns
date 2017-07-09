#include <iostream>
#include <cmath>
#include <type_traits>

template < unsigned v >
using unsigned_constant = std::integral_constant<unsigned, v>;

template < typename F >
struct argument_count : argument_count<decltype(&F::operator())> {};

template <typename R, typename C, typename... Args>
struct argument_count<R(C::*)(Args...)> : unsigned_constant<sizeof...(Args)> {};

template <typename R, typename C, typename... Args>
struct argument_count<R(C::*)(Args...) const> : unsigned_constant<sizeof...(Args)> {};

template <typename R, typename... Args>
struct argument_count<R(Args...)> : unsigned_constant<sizeof...(Args)> {};

template <typename R, typename... Args>
struct argument_count<R(*)(Args...)> : unsigned_constant<sizeof...(Args)> {};


void f(int, int, int) {}

struct A {
  void bar(int, int) {}
};

struct B {
  int operator() (double, long) { return 1; }
};

int main ()
{
  auto l = [](int) mutable {};
  auto m = [](B, A, int, double, char const *) mutable {};

  B b;

  void (*p)(int,int,int) = &f;
  
  static_assert( argument_count<decltype(f)>::value       == 3, "!");
  static_assert( argument_count<decltype(l)>::value       == 1, "!");
  static_assert( argument_count<decltype(m)>::value       == 5, "!");
  static_assert( argument_count<decltype(&A::bar)>::value == 2, "!");
  static_assert( argument_count<decltype(p)>::value       == 3, "!");
  static_assert( argument_count<decltype(b)>::value       == 2, "!");
  static_assert( argument_count<B>::value                 == 2, "!");
}
