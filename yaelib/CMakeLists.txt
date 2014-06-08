set(LIB_NAME "yaelib")

file(GLOB YAELIB_SOURCES *.cpp)
file(GLOB YAELIB_HEADERS *.hpp)

add_library(${LIB_NAME} STATIC ${YAELIB_SOURCES} ${YAELIB_HEADERS})
