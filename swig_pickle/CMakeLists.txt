cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(swig-pickle CXX)
set(CMAKE_CXX_STANDARD 11)

# Add current directory to search path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add the library

# Find Boost

find_package(Boost COMPONENTS serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND LIBRARIES ${Boost_LIBRARIES})

# Use SWIG

find_package(SWIG REQUIRED)
find_package(PythonInterp 3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)
include(${SWIG_USE_FILE})
include_directories(${PYTHON_INCLUDE_PATH})
set_source_files_properties(interface.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(interface.i PROPERTIES SWIG_FLAGS "-py3")

swig_add_module(swig_pickle python interface.i)
swig_link_libraries(swig_pickle ${PYTHON_LIBRARIES} ${LIBRARIES})

# Copy the test script to the binary dir

add_custom_target(pickle_script ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.py
                                   ${CMAKE_CURRENT_BINARY_DIR}/test.py)

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
