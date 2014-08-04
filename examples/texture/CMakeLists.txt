
set(EXECUTABLE_NAME "texture")

file(GLOB EXAMPLE_SOURCES *.cpp)
file(GLOB EXAMPLE_HEADERS *.hpp)

add_executable(${EXECUTABLE_NAME} ${EXAMPLE_SOURCES} ${EXAMPLE_HEADERS})

target_link_libraries (${EXECUTABLE_NAME} yaelib)
if (MSVC)
    target_link_libraries(${EXECUTABLE_NAME} opengl32.lib glew32.lib sfml-main-d.lib sfml-system-d.lib sfml-window-d.lib sfml-graphics-d.lib freetype.lib glew.lib jpeg.lib)
else (MSVC)
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
endif (MSVC)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${CMAKE_SOURCE_DIR}/yaelib)

configure_file(smiley.png ${CMAKE_CURRENT_BINARY_DIR}/smiley.png COPYONLY)
configure_file(smiley.png ${CMAKE_BINARY_DIR}/smiley.png COPYONLY)
