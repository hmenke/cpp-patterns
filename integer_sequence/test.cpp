#include <iostream>
#include <string>
#include <tuple>
#include <vector>

template < typename T, T... Is >
struct integer_sequence {};

// Populate integer_sequence recursively

template < typename T, typename N, typename... Is >
struct make_integer_sequence_impl
{
  using type = typename make_integer_sequence_impl<
    T,
    std::integral_constant<T,N::value-1>,
    std::integral_constant<T,N::value>,
    std::integral_constant<T,Is::value>...
    >::type;
};

template < typename T, typename... Is >
struct make_integer_sequence_impl<T,std::integral_constant<T,0>,Is...>
{
  using type = integer_sequence<T,0,Is::value...>;
};

// Interface

template < typename T, typename N >
struct make_integer_sequence_t
{
  using type = typename make_integer_sequence_impl<
    T,
    std::integral_constant<T,N::value-2>,
    std::integral_constant<T,N::value-1>
    >::type;
};

template < typename T >
struct make_integer_sequence_t<T,std::integral_constant<T,0>>
{
  using type = integer_sequence<T>;
};

template < typename T >
struct make_integer_sequence_t<T,std::integral_constant<T,1>>
{
  using type = integer_sequence<T,0>;
};

// Handy typedef

template < typename T, T N >
using make_integer_sequence =
  typename make_integer_sequence_t<T,std::integral_constant<T,N>>::type;

// From that derive index_sequence

template < size_t... Is >
using index_sequence = integer_sequence<size_t,Is...>;

template < size_t N >
using make_index_sequence = make_integer_sequence<size_t,N>;


// Call a function for each element in a tuple

template < typename T, typename F, size_t ... Is >
void for_each_impl(T&& t, F&& f, index_sequence<Is...>)
{
  using expand_type = int[];
  (void) expand_type { 0, ( (void) f(std::get<Is>(t)), 0) ... };
}

template < typename... Args, typename F >
void for_each(std::tuple<Args...> const& t, F&& f)
{
  for_each_impl(t, f, make_index_sequence<sizeof...(Args)>{});
}

// "Byte array" emulation

struct Bytes {
  std::string str;
  Bytes(char const * c) : str(c) {};
  Bytes(int i) : str(std::to_string(i)) {};
  Bytes(char c) : str(1, c) {};
};

// Surrogate template lambda

struct Visitor
{
  std::vector<Bytes>& v;
  Visitor(std::vector<Bytes>& vb) : v(vb) {};
  template < typename T >
  void operator() (T&& x) { v.push_back(x); }
};


int main()
{
  auto t = std::make_tuple(12, "abc", 10, 'c', 1);
  std::vector<Bytes> v;
  for_each(t, Visitor(v));

  for (auto const& e : v)
    std::cout << e.str << ' ';
  std::cout << '\n';
}

