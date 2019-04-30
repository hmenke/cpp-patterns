cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(detect CXX)
set(CMAKE_CXX_STANDARD 17)

# Add the executable

file(GLOB SOURCE_FILES *.cpp)

add_executable(overloaded ${SOURCE_FILES})

set_source_files_properties(${SOURCE_FILES}
  PROPERTIES COMPILE_FLAGS "${COMPILER_WARNINGS}")

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
