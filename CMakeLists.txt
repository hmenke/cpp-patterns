cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(cpp-patterns CXX)

# Add patterns

add_subdirectory(argument-count)
add_subdirectory(detection-idiom)
#add_subdirectory(enable_if) # has no CMakeLists.txt
add_subdirectory(integer_sequence)
#add_subdirectory(python_callback) # has no CMakeLists.txt
add_subdirectory(python_zmq_communication)
add_subdirectory(swig_pickle)

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
