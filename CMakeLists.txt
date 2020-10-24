cmake_minimum_required(VERSION 3.5)

project(servicelocator LANGUAGES CXX)

option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (BUILD_EXAMPLES)
add_subdirectory(examples)
endif (BUILD_EXAMPLES)

if (BUILD_TESTS)
add_subdirectory(tests)
endif (BUILD_TESTS)

