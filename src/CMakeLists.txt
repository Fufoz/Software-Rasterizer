cmake_minimum_required(VERSION 3.6)

add_library(softy STATIC
    input.cc
    texture.cc
    obj.cc
    primitives.cc
    renderer.cc
    clipper.cc
)

target_include_directories(softy PUBLIC ${SDL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../extern)
target_link_libraries(softy PUBLIC ${SDL_LIBRARIES})
