cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(cpp-patterns CXX)

if(MSVC)
  set(COMPILER_WARNINGS "/W3 /DNOMINMAX")
else()
  set(COMPILER_WARNINGS "-Wall -Wextra -Wpedantic -Weffc++")
endif()

# Add documentation

add_subdirectory(doc)

# Add patterns

add_subdirectory(argument-count)
add_subdirectory(detection-idiom)
#add_subdirectory(eigen_cuda)
#add_subdirectory(enable_if) # has no CMakeLists.txt
add_subdirectory(integer_sequence)
if(NOT MSVC)
  # MSVC immediately commits suicide when it sees those tempalates
  add_subdirectory(multi_array)
endif()
#add_subdirectory(python_callback) # has no CMakeLists.txt
add_subdirectory(python_zmq_communication)
#add_subdirectory(swig_pickle)

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
