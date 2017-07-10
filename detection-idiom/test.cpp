#include <string>
#include <type_traits>
#include <vector>

// in C++17 std::void_t
template < typename... >
using void_t = void;


template < typename T, typename Index >
using subscript_t = decltype(std::declval<T>()[std::declval<Index>()]);

template < typename, typename Index = int, typename = void_t<> >
struct has_subscript : std::false_type {};

template < typename T, typename Index >
struct has_subscript< T, Index, void_t< subscript_t<T,Index> > > : std::true_type {};


struct A
{
  void operator[](int) {}
};

struct B {};

int main ()
{
  static_assert(has_subscript< std::vector<int> >::value    == true , "!");
  static_assert(has_subscript< std::vector<double> >::value == true , "!");
  static_assert(has_subscript< A >::value                   == true , "!");
  static_assert(has_subscript< A, std::string >::value      == false, "!");
  static_assert(has_subscript< B >::value                   == false, "!");
  static_assert(has_subscript< double[5] >::value           == true , "!");
  static_assert(has_subscript< double* >::value             == true , "!");
  static_assert(has_subscript< double >::value              == false, "!");
}
