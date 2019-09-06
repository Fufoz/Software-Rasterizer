# Find the SIMPLE DIRECT MEDIA LAYER LIBRARY(a.k.a SDL)
# SDL_ROOT - variable provided by the user

# Sets the usual variables expected for find_package scripts:
# SDL_INCLUDE_DIRS - headers location
# SDL_LIBRARIES - libraries to link against
# SDL_FOUND - true if SDL was found.
# SDL_RUNTIME - OPTIONAL. only for Windows

find_path(SDL_INCLUDE_DIRS
    NAMES SDL2/SDL.h
    PATHS ${SDL_ROOT}
    HINTS ${SDL_ROOT}/include
    NO_DEFAULT_PATH
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(status " SDL Build type : ${CMAKE_BUILD_TYPE}")

set(SDL_LIB_NAME SDL2)
if(MSVC)
    set(SDL_MAIN_LIB_NAME SDL2main)
endif()
if(NOT ${SDL_LIB_NAME})
	message(STATUS "Try to look for debug libs")
    set(SDL_LIB_NAME SDL2d)
    if(MSVC)
        set(SDL_MAIN_LIB_NAME SDL2maind)
    endif()
endif()

find_library(SDL_LIBRARY 
    NAMES ${SDL_LIB_NAME}
    PATHS ${SDL_ROOT}
    HINTS ${SDL_ROOT}/lib
)

if(SDL_MAIN_LIB_NAME)
    find_library(SDL_MAIN_LIBRARY
        NAMES ${SDL_MAIN_LIB_NAME}
        PATHS ${SDL_ROOT}
        HINTS ${SDL_ROOT}/lib
    )
endif()

message(STATUS "SDL LIBS : ${SDL_LIBRARY};${SDL_MAIN_LIBRARY}")

set(SDL_LIBRARIES "${SDL_LIBRARY};${SDL_MAIN_LIBRARY}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2 DEFAULT_MSG
  SDL_INCLUDE_DIRS SDL_LIBRARIES)

#Hide in GUI
mark_as_advanced(SDL_INCLUDE_DIRS SDL_LIBRARIES)