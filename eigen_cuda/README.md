# Eigen with CUDA

## Problem

A small subset of Eigen can [run in CUDA
kernels](http://eigen.tuxfamily.org/dox/TopicCUDA.html).  This is
mainly limited to fixed size matrices but one can already do a lot
with them.  What one can *not* do is compute the inverse, i.e. this
kernel will not compile:
```cpp
template < typename MatrixType, typename ResultType >
__global__ void inverse_kernel(MatrixType * matrix, ResultType * inverse)
{
    *inverse = matrix->inverse();
}
```
with an error similar to this
```
error: calling a __host__ function(...) from a __global__ function(...) is not allowed
```

## Solution

Functions in the Eigen core are not sufficiently marked as
`__device__`.  Digging through the source code one can find that the
internal `compute_inverse` functor is sufficiently marked (which is
what computes the inverse under the hood).  This does not perform any
check whether the matrix is invertible!  It also might not work for
dynamically sized matrices.