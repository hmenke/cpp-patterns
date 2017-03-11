#include <iostream>
#include <array>
#include <vector>
#include <boost/variant.hpp>

// Vector class

template < typename Scalar, size_t n >
class Vector
{
  std::array<Scalar, n> d;
public:
  Vector() : d() {}
  
  template <typename T>
  explicit Vector(T const &v) : d()
  {
    assert(std::distance(std::begin(v), std::end(v)) == n);
    std::copy(std::begin(v), std::end(v), d.begin());
  }

  Scalar &operator[](size_t i) { return d[i]; }
  Scalar const &operator[](size_t i) const { return d[i]; }
};

// type checking using SFINAE

template < typename T >
struct is_vector
{
  static constexpr const bool value = false;
};

template < typename Scalar, size_t n >
struct is_vector < Vector < Scalar, n > >
{
  static constexpr const bool value = true;
};

typedef Vector<double, 2> Vector2d;
typedef Vector<double, 3> Vector3d;
typedef Vector<double, 4> Vector4d;

// Variant

typedef boost::variant<
  // other types
  std::vector<double>
  > Variant;

template < typename T >
typename std::enable_if<!is_vector<T>::value, T>::type
get_value(Variant const& v) { return boost::get<T>(v); }

template < typename T >
typename std::enable_if<is_vector<T>::value, T>::type
get_value(Variant const& v)
{
  return T(boost::get<std::vector<double>>(v));
}

// Testing the implementation

int main()
{
  Vector2d v2 = get_value<Vector2d>(std::vector<double>{1, 2});
  Vector3d v3 = get_value<Vector3d>(std::vector<double>{1, 2, 3});    // fails
  Vector4d v4 = get_value<Vector4d>(std::vector<double>{1, 2, 3, 4}); // fails

  std::cout << v2[0] << " " << v2[1] << "\n";
  std::cout << v3[0] << " " << v3[1] << " " << v3[2] << "\n";
  std::cout << v4[0] << " " << v4[1] << " " << v4[2] << " " << v4[3] << "\n";
}
