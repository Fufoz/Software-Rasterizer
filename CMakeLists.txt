cmake_minimum_required(VERSION 3.6)

project(soft)

if(NOT SDL_ROOT)
    message(FATAL "SDL wasn't found")
endif()

find_package(SDL REQUIRED)

#set(SDL_INCLUDE_DIRS ${SDL_ROOT}/include)
#set(SDL_IMPORT_LIBRARY ${SDL_ROOT}/lib/SDL2d.lib ${SDL_ROOT}/lib/SDL2maind.lib)

set(CMAKE_INSTALL_RPATH "$ORIGIN:${SDL_ROOT}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_executable(soft
    main.cc
    input.cc
    texture.cc
    obj.cc
    primitives.cc
    renderer.cc
    clipper.cc
)

target_include_directories(soft PUBLIC ${SDL_INCLUDE_DIRS})
target_link_libraries(soft ${SDL_LIBRARIES})

if(UNIX)
    target_compile_options(soft BEFORE PRIVATE -Wall -Werror)
elseif(MSVC)
    #target_compile_options(soft BEFORE PRIVATE /WX)
endif()


install(TARGETS soft RUNTIME DESTINATION ./bin)

if(WIN32)
    set(SDL_RUNTIME ${SDL_ROOT}/bin/SDL2d.dll)
    install(FILES ${SDL_RUNTIME} DESTINATION ./bin)
endif()