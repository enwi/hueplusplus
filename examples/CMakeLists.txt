add_executable(bridge_setup BridgeSetup.cpp)
set_property(TARGET bridge_setup PROPERTY CXX_STANDARD 14)
set_property(TARGET bridge_setup PROPERTY CXX_EXTENSIONS OFF)
target_link_libraries(bridge_setup hueplusplusstatic)
