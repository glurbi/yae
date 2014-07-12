cmake_minimum_required(VERSION 2.8)

project(yae)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-std=c++11)
endif (CMAKE_COMPILER_IS_GNUCC)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory (yaelib)
add_subdirectory (examples)

