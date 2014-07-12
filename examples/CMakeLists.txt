
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
