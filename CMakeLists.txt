cmake_minimum_required(VERSION 3.10)
project(ihd)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(include)

add_executable(ihd main.cpp)
