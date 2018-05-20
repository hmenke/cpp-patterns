cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(multi_array CXX)
set(CMAKE_CXX_STANDARD 11)

# Include current directory

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add the executable

file(GLOB SOURCE_FILES *.cpp)

add_executable(multi_array ${SOURCE_FILES})

set_source_files_properties(${SOURCE_FILES}
  PROPERTIES COMPILE_FLAGS "${COMPILER_WARNINGS}")

# Print found packages

include(FeatureSummary)
feature_summary(WHAT ALL)
