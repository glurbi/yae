
set(EXECUTABLE_NAME "example")

file(GLOB EXAMPLE_SOURCES *.cpp)
file(GLOB EXAMPLE_HEADERS *.hpp)

add_executable(${EXECUTABLE_NAME} ${EXAMPLE_SOURCES} ${EXAMPLE_HEADERS})

target_link_libraries (${EXECUTABLE_NAME} yaelib)
if (MSVC)
    target_link_libraries(${EXECUTABLE_NAME} opengl32.lib glew32.lib sfml-main-d.lib sfml-system-d.lib sfml-window-d.lib sfml-graphics-d.lib freetype.lib glew.lib jpeg.lib)
endif (MSVC)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${CMAKE_SOURCE_DIR}/yaelib)

configure_file(smiley.png ${CMAKE_CURRENT_BINARY_DIR}/smiley.png COPYONLY)
configure_file(evil.png ${CMAKE_CURRENT_BINARY_DIR}/evil.png COPYONLY)
configure_file(anonymous.ttf ${CMAKE_CURRENT_BINARY_DIR}/anonymous.ttf COPYONLY)

configure_file(${CMAKE_SOURCE_DIR}/shaders/flatShading.vert ${CMAKE_CURRENT_BINARY_DIR}/flatShading.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/flatShading.frag ${CMAKE_CURRENT_BINARY_DIR}/flatShading.frag COPYONLY)

configure_file(${CMAKE_SOURCE_DIR}/shaders/monochrome.vert ${CMAKE_CURRENT_BINARY_DIR}/monochrome.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/monochrome.frag ${CMAKE_CURRENT_BINARY_DIR}/monochrome.frag COPYONLY)

configure_file(${CMAKE_SOURCE_DIR}/shaders/texture.vert ${CMAKE_CURRENT_BINARY_DIR}/texture.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/texture.frag ${CMAKE_CURRENT_BINARY_DIR}/texture.frag COPYONLY)
