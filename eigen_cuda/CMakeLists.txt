cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(eigen-cuda-inverse CXX)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCE_FILES *.cu)

# Add current directory to search path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find CUDA

find_package(CUDA REQUIRED)

find_path(EIGEN_INCLUDE_DIR
  NAMES signature_of_eigen3_matrix_library
  HINTS /usr/local/include/eigen3 /usr/include/eigen3)
if(NOT EIGEN_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find Eigen3!")
else()
  message(STATUS "Found Eigen3: ${EIGEN_INCLUDE_DIR}")
  include_directories(${EIGEN_INCLUDE_DIR})
endif()


# Add executable
cuda_add_executable(cuinv ${SOURCE_FILES})

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
