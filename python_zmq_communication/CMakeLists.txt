cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(python-zmq-communication CXX)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Add current directory to search path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add the library

file(GLOB SOURCE_FILES *.cpp)

set_source_files_properties(${SOURCE_FILES}
  PROPERTIES COMPILE_FLAGS "${COMPILER_WARNINGS}")

add_library(zmq_cpp_backend SHARED ${SOURCE_FILES})

# Find OpenMP

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Find ZMQ and Boost

find_package(ZeroMQ)
if(ZeroMQ_FOUND)
  include_directories(${ZeroMQ_INCLUDE_DIR})
  list(APPEND LIBRARIES "libzmq")
  message(STATUS "ZeroMQ found: ${ZeroMQ_LIBRARY}")
else()
  find_path(ZMQ_INCLUDE_DIR NAMES zmq.h)
  find_library(ZMQ_LIBRARY NAMES zmq libzmq)
  message(STATUS "Attempting to find ZeroMQ manually: ${ZMQ_LIBRARY}")
  include_directories(${ZMQ_INCLUDE_DIR})
  list(APPEND LIBRARIES ${ZMQ_LIBRARY})
endif()

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND LIBRARIES ${Boost_LIBRARIES})

target_link_libraries(zmq_cpp_backend ${LIBRARIES})

# Use SWIG

find_package(SWIG REQUIRED)
find_package(PythonInterp 3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)
include(${SWIG_USE_FILE})
include_directories(${PYTHON_INCLUDE_PATH})
set_source_files_properties(interface.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(interface.i PROPERTIES SWIG_FLAGS "-py3")

swig_add_module(python_zmq_communication python interface.i)
swig_link_libraries(python_zmq_communication zmq_cpp_backend ${PYTHON_LIBRARIES})

# Copy the test script to the binary dir

add_custom_target(zmq_script ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.py
                                   ${CMAKE_CURRENT_BINARY_DIR}/test.py)

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
