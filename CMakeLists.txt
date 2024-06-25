#
# Copyright 2024 Ipsolon Research
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

cmake_minimum_required(VERSION 3.10)
project(ihd)

set(CMAKE_CXX_STANDARD 17)


set(BOOST_REQUIRED_COMPONENTS
        program_options
        system
        thread
)
find_package(Boost "1.74.0" REQUIRED ${BOOST_REQUIRED_COMPONENTS})


file(GLOB INCLUDE_FILES include/*.h include/*.hpp)
file(GLOB LIB_FILES lib/*.c lib/*.cpp)

include_directories(include)
link_directories(${Boost_LIBRARY_DIRS})

add_executable(ihd main.cpp ${INCLUDE_FILES} ${LIB_FILES})

target_link_libraries(ihd  -luhd ${Boost_LIBRARIES})
