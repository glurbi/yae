cmake_minimum_required(VERSION 2.8)

project(yae)

cmake_policy(SET CMP0022 OLD)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-std=c++11)
    add_definitions(-std=c++1y)
endif (CMAKE_COMPILER_IS_GNUCC)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED glew32)
find_package(SDL2 REQUIRED)
find_package(GTEST REQUIRED)

include_directories(${SDL2_INCLUDE_PATH})
include_directories(${GLEW_INCLUDE_PATH})

add_subdirectory (src)
add_subdirectory (tests)
add_subdirectory (examples)

