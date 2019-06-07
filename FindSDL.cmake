# Find the SIMPLE DIRECT MEDIA LAYER LIBRARY(a.k.a SDL)
# SDL_ROOT - variable provided by the user

# Sets the usual variables expected for find_package scripts:
# SDL_INCLUDE_DIRS - headers location
# SDL_LIBRARIES - libraries to link against
# SDL_FOUND - true if Face SDK was found.

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

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    set(SDL_LIB_NAME SDL2)
else()
    set(SDL_LIB_NAME SDL2d)
endif()

find_library(SDL_LIBRARIES 
    NAMES ${SDL_LIB_NAME}
    PATHS ${SDL_ROOT}
    hints ${SDL_ROOT}/lib
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2 DEFAULT_MSG
  SDL_INCLUDE_DIRS SDL_LIBRARIES)

#Hide in GUI
mark_as_advanced(SDL_INCLUDE_DIRS SDL_LIBRARIES)