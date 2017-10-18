# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
    RESULT_VARIABLE result
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/hueplusplus/test/googletest-download" )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    RESULT_VARIABLE result
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/hueplusplus/test/googletest-download" )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src EXCLUDE_FROM_ALL
                 ${CMAKE_BINARY_DIR}/googletest-build EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include" EXCLUDE_FROM_ALL)
endif()

file(GLOB TEST_SOURCES test_*)

add_executable(HuePlusPlus_Test ${TEST_SOURCES} ${hueplusplus_SOURCES})
target_link_libraries(HuePlusPlus_Test gtest gmock)
# prevent Main.cpp from defining main()
target_compile_definitions(HuePlusPlus_Test PUBLIC MAIN_CPP_NO_MAIN_FUNCTION)
target_include_directories(HuePlusPlus_Test PUBLIC ${GTest_INCLUDE_DIRS})
target_include_directories(HuePlusPlus_Test PUBLIC ${HuePlusPlus_INCLUDE_DIR})
set_property(TARGET HuePlusPlus_Test PROPERTY CXX_STANDARD 14)
set_property(TARGET HuePlusPlus_Test PROPERTY CXX_EXTENSIONS OFF)
