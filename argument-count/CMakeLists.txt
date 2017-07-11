cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(argcnt CXX)
set(CMAKE_CXX_STANDARD 11)

# Add the executable

file(GLOB SOURCE_FILES *.cpp)

add_executable(argcnt ${SOURCE_FILES})

set_source_files_properties(${SOURCE_FILES}
  PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Weffc++")

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
