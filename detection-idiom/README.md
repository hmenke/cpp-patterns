# Detection Idiom

CppCon 2014: Walter E. Brown "Modern Template Metaprogramming: A Compendium, Part II"

https://www.youtube.com/watch?v=a0FliKwcwXE&t=29m7s

## Problem

We can use the `void_t` trick shown in the above video to detect
whether a type has a member function, overloaded operator, etc.  In
this example we want to detect whether a type has a subscript operator
with subscript `int`.

## Solution

The `void_t` uses expression SFINAE to detect the existence of
something.  The code is ultra short.
```cpp
template < typename T, typename Index >
using subscript_t = decltype(std::declval<T>()[std::declval<Index>()]);

template < typename, typename Index = int, typename = void_t<> >
struct has_subscript : std::false_type {};

template < typename T, typename Index >
struct has_subscript< T, Index, void_t< subscript_t<T,Index> > > : std::true_type {};
```
We could make it even shorter by omitting `subscript_t`.  For
explanation watch the video.