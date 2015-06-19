#
# Try to find SDL2 library and include path.
# Once done this will define
#
# SDL2_FOUND
# SDL2_INCLUDE_PATH
# SDL2_LIBRARY
# 

IF (WIN32)

	FIND_PATH(
        SDL2_INCLUDE_PATH SDL.h
		$ENV{PROGRAMFILES}/SDL2/include
        "D:/Program Files (x86)/SDL2-2.0.3/include"
		DOC "The directory where SDL.h resides")

    FIND_LIBRARY(
        SDL2_LIBRARY
        NAMES SDL2 SDL2 SDL232 SDL232s
        PATHS
        "D:/Program Files (x86)/SDL2-2.0.3/lib/x86"
        $ENV{PROGRAMFILES}/SDL2/lib
        DOC "The SDL2 library")

    FIND_LIBRARY(
        SDL2_IMAGE_LIBRARY
        NAMES SDL2_image 
        PATHS
        "D:/Program Files (x86)/SDL2-2.0.3/lib/x86"
        $ENV{PROGRAMFILES}/SDL2/lib
        DOC "The SDL2_image library")
        
ELSE (WIN32)

	FIND_PATH(
        SDL2_INCLUDE_PATH SDL.h
		/usr/include
		DOC "The directory where SDL.h resides")

	FIND_LIBRARY(
        SDL2_LIBRARY
		NAMES SDL2 SDL2
		PATHS
		/usr/lib64
		/usr/lib
		DOC "The SDL2 library")

	FIND_LIBRARY(
        SDL2_IMAGE_LIBRARY
		NAMES SDL2_image
		PATHS
		/usr/lib64
		/usr/lib
		DOC "The SDL2_image library")
        
ENDIF (WIN32)

SET(SDL2_FOUND "NO")
IF (SDL2_INCLUDE_PATH AND SDL2_LIBRARY AND SDL2_IMAGE_LIBRARY)
	SET(SDL2_FOUND "YES")
ENDIF (SDL2_INCLUDE_PATH AND SDL2_LIBRARY AND SDL2_IMAGE_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_LIBRARY SDL2_INCLUDE_PATH)
find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_INCLUDE_PATH)
