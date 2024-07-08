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

find_package(UHD "4.1.0.5" EXACT REQUIRED )

file(GLOB LIB_FILES lib/*.c
                    lib/*.cpp
                    lib/chameleon/*.c
                    lib/chameleon/*.cpp)

include_directories(include lib/chameleon)
link_directories(${Boost_LIBRARY_DIRS})

add_executable(ihd_recv host_main.cpp ${LIB_FILES})
add_executable(target_sim target_sim_main.cpp ${LIB_FILES})

target_link_libraries(ihd_recv -luhd ${Boost_LIBRARIES})
target_link_libraries(target_sim -luhd ${Boost_LIBRARIES})

add_library(ihd SHARED ${LIB_FILES})

install(TARGETS ihd
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
            IMMEDIATE @ONLY)

    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
