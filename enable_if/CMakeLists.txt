cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(enable_if CXX)
set(CMAKE_CXX_STANDARD 11)

# Add the executable

file(GLOB SOURCE_FILES *.cpp)

add_executable(enable_if ${SOURCE_FILES})

set_source_files_properties(${SOURCE_FILES}
  PROPERTIES COMPILE_FLAGS "${COMPILER_WARNINGS}")

# Find Boost

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
