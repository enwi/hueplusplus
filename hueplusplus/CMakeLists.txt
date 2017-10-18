file(GLOB hueplusplus_HEADERS *.h *.hpp)
file(GLOB hueplusplus_SOURCES *.cpp)

add_library(HuePlusPlusShared SHARED ${hueplusplus_SOURCES})
set_property(TARGET HuePlusPlusShared PROPERTY CXX_STANDARD 14)
set_property(TARGET HuePlusPlusShared PROPERTY CXX_EXTENSIONS OFF)

add_library(HuePlusPlusStatic STATIC ${hueplusplus_SOURCES})
set_property(TARGET HuePlusPlusStatic PROPERTY CXX_STANDARD 14)
set_property(TARGET HuePlusPlusStatic PROPERTY CXX_EXTENSIONS OFF)
