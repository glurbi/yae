cmake_minimum_required(VERSION 2.8)

project(yae)

if (CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-std=c++11)
endif (CMAKE_COMPILER_IS_GNUCC)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory (yaelib)
add_subdirectory (examples)

configure_file(shaders/flatShading.vert ${CMAKE_CURRENT_BINARY_DIR}/flatShading.vert COPYONLY)
configure_file(shaders/flatShading.frag ${CMAKE_CURRENT_BINARY_DIR}/flatShading.frag COPYONLY)

configure_file(shaders/monochrome.vert ${CMAKE_CURRENT_BINARY_DIR}/monochrome.vert COPYONLY)
configure_file(shaders/monochrome.frag ${CMAKE_CURRENT_BINARY_DIR}/monochrome.frag COPYONLY)

configure_file(shaders/texture.vert ${CMAKE_CURRENT_BINARY_DIR}/texture.vert COPYONLY)
configure_file(shaders/texture.frag ${CMAKE_CURRENT_BINARY_DIR}/texture.frag COPYONLY)
