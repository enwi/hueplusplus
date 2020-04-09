cmake_minimum_required(VERSION 3.2)

if(${CMAKE_VERSION} VERSION_LESS 3.11)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
	cmake_policy(VERSION 3.11)
endif()

# Add cmake dir to module path, so Find*.cmake can be found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

project(hueplusplus LANGUAGES CXX)

# options to set
option(hueplusplus_TESTS "Build tests" OFF)

# get the correct installation directory for add_library() to work
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/hueplusplus)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# target for uninstall
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

# if we are on a apple machine this is needed
if (1 AND APPLE)
	set(CMAKE_MACOSX_RPATH 1)
endif()

add_subdirectory(hueplusplus)
