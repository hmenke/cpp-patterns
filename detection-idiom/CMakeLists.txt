cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(detect CXX)

# Add the executable

file(GLOB SOURCE_FILES *.cpp)

add_executable(detect ${SOURCE_FILES})

set_source_files_properties(${SOURCE_FILES}
  PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Weffc++")

# Use C++11

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(FATAL "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
