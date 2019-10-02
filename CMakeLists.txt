cmake_minimum_required(VERSION 3.6)
project(Softy)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN:${SDL_ROOT}/lib")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

if(NOT SDL_ROOT)
    message(WARNING "SDL_ROOT was not set, start looking sdl lib in default paths..")
endif()
find_package(SDL REQUIRED)

add_subdirectory(src)
add_subdirectory(examples)
