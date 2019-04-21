cmake_minimum_required(VERSION 3.6)

project(soft)
if(SDL_ROOT)
    set(SDL_INCLUDE_DIRS ${SDL_ROOT}/include)
    set(SDL_DEBUG_LIBS "${SDL_ROOT}/lib/SDL2maind.lib;${SDL_ROOT}/lib/SDL2-staticd.lib" )
    #set(SDL_DEBUG_LIBS "${SDL_ROOT}/lib/SDL2d.lib" )
else()
    message(FATAL_ERROR "set SDL_ROOT install directory!")
endif()


add_executable(soft
    main.cpp
    vec.cpp
    obj.cpp
)

target_include_directories(soft PUBLIC ${SDL_INCLUDE_DIRS})
target_link_libraries(soft  user32;gdi32;winmm;imm32;ole32;oleaut32;version;uuid;advapi32;shell32;dinput8 ${SDL_DEBUG_LIBS})

message("includedirs are ${SDL_INCLUDE_DIRS} libs: ${SDL_ROOT}/lib/SDL2maind.lib ${SDL_DEBUG_LIBS}" )
install(TARGETS soft RUNTIME DESTINATION ./)