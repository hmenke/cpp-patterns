#include <iostream>
#include <string>
#include <tuple>
#include <vector>

template < typename T, T... I >
struct index_sequence {};

template < typename T, T N, T... I>
struct make_index_sequence : public make_index_sequence<T, N - 1, N - 1, I...> {};

template < typename T, T... I>
struct make_index_sequence<T, 0, I...> : public index_sequence<T, I...> {};

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

