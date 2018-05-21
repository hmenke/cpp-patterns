cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(device_matrix CXX)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCE_FILES *.cu)

# Add current directory to search path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find CUDA

find_package(CUDA REQUIRED)

# Add executable
cuda_add_executable(device_matrix_test ${SOURCE_FILES})
target_link_libraries(device_matrix_test
  ${CUDA_CUBLAS_LIBRARIES}
  ${CUDA_cusolver_LIBRARY})

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
