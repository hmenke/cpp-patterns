# enable_if

## Problem

Was have a template class `Vector` which can be constructed from a
`std::vector`.  A `boost::variant` which holds a `std::vector` should
be convertible to a `Vector`.  Therefore we write a custom `get_value`
function.

    template < typename T >
    T get_value(Variant const& v) { return boost::get<T>(v); }

This way we cannot directly construct a `Vector`, because only
`std::vector` is a valid member of the variant.  This means that

    typedef Vector<double, 3> Vector3d;

    auto vector = get_value<Vector3d>(variant);

will fail with `bad_get`.  We could write a specialization for
`Vector3d` to make it work.

    template < >
    Vector3d get_value<Vector3d>(Variant const& v)
    {
      return Vector3d(boost::get<std::vector<double>>(v));
    }

Unfortunately, this is not generic and means that we would have to
write a specialization for all possinle instantiations of `Vector`.

## Solution

We use SFINAE for type checking and implement a type checking `struct`.

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

Then we use `std::enable_if` to conditionally instaniate one of the
`get_value` functions.

    template < typename T >
    typename std::enable_if<!is_vector<T>::value, T>::type
    get_value(Variant const& v) { return boost::get<T>(v); }

    template < typename T >
    typename std::enable_if<is_vector<T>::value, T>::type
    get_value(Variant const& v)
    {
      return T(boost::get<std::vector<double>>(v));
    }

