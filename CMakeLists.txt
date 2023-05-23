cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(solanaceae)

#TODO: seperate into host and client
add_library(solanaceae_plugin
	./solanaceae/plugin/solana_plugin_v1.h
	./solanaceae/plugin/plugin.hpp
	./solanaceae/plugin/plugin.cpp
	./solanaceae/plugin/plugin_manager.hpp
	./solanaceae/plugin/plugin_manager.cpp
)

target_include_directories(solanaceae_plugin PUBLIC .)
target_compile_features(solanaceae_plugin PUBLIC cxx_std_17)
target_link_libraries(solanaceae_plugin PUBLIC
	#solanaceae_core
	${CMAKE_DL_LIBS}
)

