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

if(WIN32)
    set(hueplusplus_SOURCES
        ${hueplusplus_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/winHttpHandler.cpp
    )
endif()
if(UNIX)
    set(hueplusplus_SOURCES
        ${hueplusplus_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/linHttpHandler.cpp
    )
endif()

add_library(hueplusplusshared SHARED ${hueplusplus_SOURCES})
set_property(TARGET hueplusplusshared PROPERTY CXX_STANDARD 14)
set_property(TARGET hueplusplusshared PROPERTY CXX_EXTENSIONS OFF)

add_library(hueplusplusstatic STATIC ${hueplusplus_SOURCES})
set_property(TARGET hueplusplusstatic PROPERTY CXX_STANDARD 14)
set_property(TARGET hueplusplusstatic PROPERTY CXX_EXTENSIONS OFF)
install(TARGETS hueplusplusstatic DESTINATION lib)
install(FILES ${hueplusplus_HEADERS} DESTINATION include)

if(hueplusplus_TESTS)
    set(HuePlusPlus_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory("test")
endif()
