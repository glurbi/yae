
set(PROGRAM_NAME "mandelbrot")

file(GLOB PROGRAM_SOURCES *.cpp)
file(GLOB PROGRAM_HEADERS *.hpp)

add_executable(${PROGRAM_NAME} ${PROGRAM_SOURCES} ${PROGRAM_HEADERS})

target_link_libraries(${PROGRAM_NAME} ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} yaelib)

set_target_properties(${PROGRAM_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${CMAKE_SOURCE_DIR}/src)

