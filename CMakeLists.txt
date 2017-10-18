cmake_minimum_required(VERSION 2.8.3)
project(hueplusplus)

option(hueplusplus_TESTS "Build tests" OFF)

if (1 AND APPLE)
	set(CMAKE_MACOSX_RPATH 1)
endif()

add_subdirectory(hueplusplus)
