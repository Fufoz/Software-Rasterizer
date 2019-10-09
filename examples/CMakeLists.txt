cmake_minimum_required(VERSION 3.6)

function(create_demo targetName source)
	add_executable(${targetName} ${source})
	target_include_directories(${targetName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
	target_link_libraries(${targetName} PRIVATE softy)
	install(TARGETS ${targetName} RUNTIME DESTINATION bin)
endfunction()

create_demo(depth depth.cc)
create_demo(parallax parallax.cc)
create_demo(shadingModels shadingModels.cc)

if(WIN32)
	#grab all example's dependent libraries for windows platform
	file(GLOB SDL_RUNTIME ${SDL_ROOT}/bin/*.dll)
	install(FILES ${SDL_RUNTIME} DESTINATION ./bin)
endif()