# Argument Counting

## Problem

Under certain circumstances it might be necessary to count how many
arguments a function is taking.  In my specific use case I wanted to
infer the dimensionality of an integral from the function signature.
That is to say, a funtion with the signature
```cpp
double f(double x, double y, double z);
```
should most probably be integrated over three-dimensional space.

## Solution

We use nifty partial spezializations of a templated struct for the
various types of signatures.  The example is incomplete in the sense
that not all combinations of const and volatile qualifiers are
exhausted.