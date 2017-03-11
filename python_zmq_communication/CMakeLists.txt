cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(swig-test CXX)

# Add current directory to search path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add the library

file(GLOB SOURCE_FILES *.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

add_library(backend SHARED ${SOURCE_FILES})

# Find ZMQ

find_library(ZMQ_LIBRARY zmq)
list(APPEND LIBRARIES ${ZMQ_LIBRARY})

target_link_libraries(backend ${LIBRARIES})

# Use SWIG

find_package(SWIG REQUIRED)
find_package(PythonInterp 3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)
include(${SWIG_USE_FILE})
include_directories(${PYTHON_INCLUDE_PATH})
set_source_files_properties(interface.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(interface.i PROPERTIES SWIG_FLAGS "-py3")

swig_add_module(example python interface.i)
swig_link_libraries(example backend ${PYTHON_LIBRARIES})

# Use C++11

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(FATAL "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Copy the test script to the binary dir

add_custom_target(test_script ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/test.py
                                   ${CMAKE_BINARY_DIR}/test.py)

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
