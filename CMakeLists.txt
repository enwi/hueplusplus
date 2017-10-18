cmake_minimum_required(VERSION 2.8.3)
project(hueplusplus)

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

option(hueplusplus_TESTS "Build tests" OFF)

if (1 AND APPLE)
	set(CMAKE_MACOSX_RPATH 1)
endif()

add_subdirectory(hueplusplus)
