cmake_minimum_required(VERSION 3.5)

project(servicelocator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("../include")

add_executable(servicelocator main.cpp)
