
set(PROGRAM_NAME "texture")

file(GLOB PROGRAM_SOURCES *.cpp)
file(GLOB PROGRAM_HEADERS *.hpp)

add_executable(${PROGRAM_NAME} ${PROGRAM_SOURCES} ${PROGRAM_HEADERS})

target_link_libraries (${PROGRAM_NAME} yaelib)
if (MSVC)
    target_link_libraries(${PROGRAM_NAME} opengl32.lib glew32.lib sfml-main-d.lib sfml-system-d.lib sfml-window-d.lib sfml-graphics-d.lib freetype.lib glew.lib jpeg.lib)
else (MSVC)
    target_link_libraries(${PROGRAM_NAME} ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
endif (MSVC)
set_target_properties(${PROGRAM_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${CMAKE_SOURCE_DIR}/yaelib)

configure_file(smiley.png ${CMAKE_CURRENT_BINARY_DIR}/smiley.png COPYONLY)
configure_file(smiley.png ${CMAKE_BINARY_DIR}/smiley.png COPYONLY)
