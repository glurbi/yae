#
# Try to find SDL2 library and include path.
# Once done this will define
#
# GTEST_FOUND
# GTEST_INCLUDE_PATH
# GTEST_LIBRARY
# GTEST_MAIN_LIBRARY
# 

IF (WIN32)

    FIND_PATH(
        GTEST_INCLUDE_PATH gtest/gtest.h
        $ENV{PROGRAMFILES}/gtest-1.7.0/include
        "D:/Program Files (x86)/gtest-1.7.0/include"
        DOC "The gtest include directory")

    FIND_LIBRARY(
        GTEST_LIBRARY
        NAMES gtest
        PATHS
        "D:/Program Files (x86)/gtest-1.7.0"
        $ENV{PROGRAMFILES}/gtest-1.7.0/lib
        DOC "The gtest library")
        
    FIND_LIBRARY(
        GTEST_MAIN_LIBRARY
        NAMES gtest_main
        PATHS
        "D:/Program Files (x86)/gtest-1.7.0"
        $ENV{PROGRAMFILES}/gtest-1.7.0/lib
        DOC "The gtest_main library")

ELSE (WIN32)

	FIND_PATH(
        GTEST_INCLUDE_PATH gtest.h
        /usr/include
        DOC "The gtest include directory")

	FIND_LIBRARY(
        GTEST_LIBRARY
        NAMES gtest
        PATHS
        /usr/lib
        DOC "The gtest library")
        
	FIND_LIBRARY(
        GTEST_MAIN_LIBRARY
        NAMES gtest_main
        PATHS
        /usr/lib
        DOC "The gtest_main library")
        
ENDIF (WIN32)

SET(GTEST_FOUND "NO")
IF (GTEST_INCLUDE_PATH AND GTEST_LIBRARY)
	SET(GTEST_FOUND "YES")
ENDIF (GTEST_INCLUDE_PATH AND GTEST_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTEST DEFAULT_MSG GTEST_INCLUDE_PATH)
