cmake_minimum_required(VERSION 2.8)

set(EXECUTABLE_NAME "yaedemo")

project(${EXECUTABLE_NAME})

add_subdirectory (yaelib)

file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.hpp)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-std=c++11)
endif (CMAKE_COMPILER_IS_GNUCC)

add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS})
target_link_libraries (${EXECUTABLE_NAME} yaelib)
if (MSVC)
    target_link_libraries(${EXECUTABLE_NAME} opengl32.lib glew32.lib sfml-main-d.lib sfml-system-d.lib sfml-window-d.lib sfml-graphics-d.lib freetype.lib glew.lib jpeg.lib)
endif (MSVC)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/yaelib)

configure_file(smiley.png ${CMAKE_CURRENT_BINARY_DIR}/smiley.png COPYONLY)
configure_file(evil.png ${CMAKE_CURRENT_BINARY_DIR}/evil.png COPYONLY)
configure_file(anonymous.ttf ${CMAKE_CURRENT_BINARY_DIR}/anonymous.ttf COPYONLY)
configure_file(flatShading.frag ${CMAKE_CURRENT_BINARY_DIR}/flatShading.frag COPYONLY)
configure_file(monochrome.frag ${CMAKE_CURRENT_BINARY_DIR}/monochrome.frag COPYONLY)
configure_file(texture.frag ${CMAKE_CURRENT_BINARY_DIR}/texture.frag COPYONLY)
configure_file(flatShading.vert ${CMAKE_CURRENT_BINARY_DIR}/flatShading.vert COPYONLY)
configure_file(monochrome.vert ${CMAKE_CURRENT_BINARY_DIR}/monochrome.vert COPYONLY)
configure_file(texture.vert ${CMAKE_CURRENT_BINARY_DIR}/texture.vert COPYONLY)
