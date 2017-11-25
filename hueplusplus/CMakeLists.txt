file(GLOB hueplusplus_HEADERS include/*.h include/*.hpp)
set(hueplusplus_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ExtendedColorHueStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ExtendedColorTemperatureStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Hue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HueLight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleBrightnessStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleColorHueStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleColorTemperatureStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UPnP.cpp
)

# on windows we want to compile the winHttpHandler
if(WIN32)
    set(hueplusplus_SOURCES
        ${hueplusplus_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/winHttpHandler.cpp
    )
endif()
# whereas on linux we want the linHttpHandler
if(UNIX)
    set(hueplusplus_SOURCES
        ${hueplusplus_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/linHttpHandler.cpp
    )
endif()

# hueplusplus shared library
add_library(hueplusplusshared SHARED ${hueplusplus_SOURCES})
set_property(TARGET hueplusplusshared PROPERTY CXX_STANDARD 14)
set_property(TARGET hueplusplusshared PROPERTY CXX_EXTENSIONS OFF)
install(TARGETS hueplusplusshared DESTINATION lib)

# hueplusplus static library
add_library(hueplusplusstatic STATIC ${hueplusplus_SOURCES})
set_property(TARGET hueplusplusstatic PROPERTY CXX_STANDARD 14)
set_property(TARGET hueplusplusstatic PROPERTY CXX_EXTENSIONS OFF)
install(TARGETS hueplusplusstatic DESTINATION lib)
install(FILES ${hueplusplus_HEADERS} DESTINATION include/hueplusplus)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE hueplusplus)
# Create the hueplusplus-config.cmake
configure_file (hueplusplus-config.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hueplusplus-config.cmake" @ONLY)
# Install hueplusplus-config.cmake
install(FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hueplusplus-config.cmake" DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# if the user decided to use tests add the subdirectory
if(hueplusplus_TESTS)
    set(HuePlusPlus_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory("test")
endif()
