cmake_minimum_required(VERSION 3.6)

project(soft)

if(NOT SDL_ROOT)
    message(FATAL "SDL wasn't found")
endif()

find_package(SDL REQUIRED)

set(CMAKE_INSTALL_RPATH "$ORIGIN:${SDL_ROOT}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library(soft STATIC
    input.cc
    texture.cc
    obj.cc
    primitives.cc
    renderer.cc
    clipper.cc
)

target_include_directories(soft PUBLIC ${SDL_INCLUDE_DIRS})
target_link_libraries(soft PUBLIC ${SDL_LIBRARIES})
add_subdirectory(examples)

if(WIN32)
    set(SDL_RUNTIME ${SDL_ROOT}/bin/SDL2d.dll)
    install(FILES ${SDL_RUNTIME} DESTINATION ./bin)
endif()