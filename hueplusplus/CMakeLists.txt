file(GLOB hueplusplus_HEADERS *.h *.hpp)
file(GLOB hueplusplus_SOURCES *.cpp)

add_library(hueplusplusshared SHARED ${hueplusplus_SOURCES})
set_property(TARGET hueplusplusshared PROPERTY CXX_STANDARD 14)
set_property(TARGET hueplusplusshared PROPERTY CXX_EXTENSIONS OFF)

add_library(hueplusplusstatic STATIC ${hueplusplus_SOURCES})
set_property(TARGET hueplusplusstatic PROPERTY CXX_STANDARD 14)
set_property(TARGET hueplusplusstatic PROPERTY CXX_EXTENSIONS OFF)
install(TARGETS hueplusplusstatic DESTINATION lib)
install(FILES ${hueplusplus_HEADERS} DESTINATION include)
