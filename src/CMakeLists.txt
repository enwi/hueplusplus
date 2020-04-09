file(GLOB hueplusplus_HEADERS include/*.h include/*.hpp)
set(hueplusplus_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/BaseHttpHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ExtendedColorHueStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ExtendedColorTemperatureStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Hue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HueCommandAPI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HueDeviceTypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HueException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HueLight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleBrightnessStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleColorHueStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleColorTemperatureStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UPnP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils.cpp
)

# on windows we want to compile the WinHttpHandler
if(WIN32)
    set(hueplusplus_SOURCES
        ${hueplusplus_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/WinHttpHandler.cpp PARENT_SCOPE
    )
endif()
# whereas on linux we want the LinHttpHandler
if(UNIX)
    set(hueplusplus_SOURCES
        ${hueplusplus_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/LinHttpHandler.cpp PARENT_SCOPE
    )
endif()
if(ESP_PLATFORM)
    set(hueplusplus_SOURCES
        ${hueplusplus_SOURCES}
       ${CMAKE_CURRENT_SOURCE_DIR}/LinHttpHandler.cpp PARENT_SCOPE
    )
endif()

# Propagate to parent scope for tests
set(hueplusplus_SOURCES ${hueplusplus_SOURCES} PARENT_SCOPE)

# hueplusplus shared library
add_library(hueplusplusshared SHARED ${hueplusplus_SOURCES})
target_compile_features(hueplusplusshared PUBLIC cxx_std_14)
target_include_directories(hueplusplusshared PUBLIC $<BUILD_INTERFACE:${hueplusplus_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
install(TARGETS hueplusplusshared DESTINATION lib)

# hueplusplus static library
add_library(hueplusplusstatic STATIC ${hueplusplus_SOURCES})
target_compile_features(hueplusplusstatic PUBLIC cxx_std_14)
install(TARGETS hueplusplusstatic DESTINATION lib)
target_include_directories(hueplusplusstatic PUBLIC $<BUILD_INTERFACE:${hueplusplus_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
install(FILES ${hueplusplus_HEADERS} DESTINATION include/hueplusplus)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE hueplusplus)
# Create the hueplusplus-config.cmake
configure_file ("${hueplusplus_SOURCE_DIR}/cmake/hueplusplus-config.cmake.in" "${hueplusplus_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hueplusplus-config.cmake" @ONLY)
# Install hueplusplus-config.cmake
install(FILES "${hueplusplus_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hueplusplus-config.cmake" DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
