cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(device_matrix CXX)
set(CMAKE_CXX_STANDARD 11)

# Add current directory to search path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)
find_package(CUDA REQUIRED)

set(SOURCE_FILES test.cu)
cuda_add_library(test_gpu ${SOURCE_FILES})
target_link_libraries(test_gpu
  ${CUDA_CUBLAS_LIBRARIES}
  ${CUDA_cusolver_LIBRARY})

set(SOURCE_FILES test.cpp)
add_library(test_cpu ${SOURCE_FILES})
target_link_libraries(test_cpu
  ${BLAS_LIBRARIES}
  ${LAPACK_LIBRARIES})

set(SOURCE_FILES runner.cpp)
add_executable(device_matrix_test ${SOURCE_FILES})
target_link_libraries(device_matrix_test test_cpu test_gpu)

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
