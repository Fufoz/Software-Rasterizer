cmake_minimum_required(VERSION 3.6)

function(create_demo targetName source)
    add_executable(${targetName} ${source})
    target_include_directories(${targetName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
    target_link_libraries(${targetName} PRIVATE soft)
    install(TARGETS ${targetName} RUNTIME DESTINATION bin)
endfunction()

create_demo(depth depth.cc)
create_demo(parallax parallax.cc)
create_demo(shadingModels shadingModels.cc)