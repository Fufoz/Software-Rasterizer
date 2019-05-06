cmake_minimum_required(VERSION 3.6)

project(soft)

if(NOT SDL_ROOT)
    message(FATAL "SDL wasn't found")
endif()

set(SDL_INCLUDE_DIRS ${SDL_ROOT}/include)
set(SDL_IMPORT_LIBRARY ${SDL_ROOT}/lib/SDL2d.lib ${SDL_ROOT}/lib/SDL2maind.lib)
set(SDL_RUNTIME ${SDL_ROOT}/bin/SDL2d.dll)

add_executable(soft
    main.cc
    obj.cc
    primitives.cc
    renderer.cc
)

target_include_directories(soft PUBLIC ${SDL_INCLUDE_DIRS})
target_link_libraries(soft ${SDL_IMPORT_LIBRARY})

install(TARGETS soft RUNTIME DESTINATION ./)
install(FILES ${SDL_RUNTIME} DESTINATION ./)