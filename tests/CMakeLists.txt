
set(PROGRAM_NAME "yae_test")

file(GLOB PROGRAM_SOURCES *.cpp)

add_executable(${PROGRAM_NAME} ${PROGRAM_SOURCES})

target_link_libraries(${PROGRAM_NAME} ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} ${GTEST} ${GTEST_MAIN} yaelib)

set_target_properties(${PROGRAM_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${GTEST_INCLUDE_PATH})
include_directories(${CMAKE_SOURCE_DIR}/src)

