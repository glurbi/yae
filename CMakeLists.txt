cmake_minimum_required(VERSION 2.8)

project(yae)

cmake_policy(SET CMP0022 OLD)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-std=c++11)
endif (CMAKE_COMPILER_IS_GNUCC)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(SFML 2.1 REQUIRED system window graphics network audio)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED glew32)

add_subdirectory (yaelib)
add_subdirectory (examples)

