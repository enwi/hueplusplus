# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/hueplusplus/test/googletest-download"
)
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/hueplusplus/test/googletest-download"
)
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src EXCLUDE_FROM_ALL
                 ${CMAKE_BINARY_DIR}/googletest-build EXCLUDE_FROM_ALL
)

# define all test sources
set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_BaseHttpHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ExtendedColorHueStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ExtendedColorTemperatureStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_Hue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_HueLight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_HueCommandAPI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_Main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_SimpleBrightnessStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_SimpleColorHueStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_SimpleColorTemperatureStrategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_UPnP.cpp
)

set(HuePlusPlus_INCLUDE_DIR "${hueplusplus_SOURCE_DIR}/include")

# test executable
add_executable(test_HuePlusPlus ${TEST_SOURCES} ${hueplusplus_SOURCES})
if(DO_CLANG_TIDY)
	set_target_properties(test_HuePlusPlus PROPERTIES CXX_CLANG_TIDY ${DO_CLANG_TIDY})
endif()
target_compile_features(test_HuePlusPlus PUBLIC cxx_std_14)
set_property(TARGET test_HuePlusPlus PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(test_HuePlusPlus gtest gmock)
target_include_directories(test_HuePlusPlus PUBLIC ${GTest_INCLUDE_DIRS})
target_include_directories(test_HuePlusPlus PUBLIC ${HuePlusPlus_INCLUDE_DIR})
# add custom target to make it simple to run the tests
add_custom_target("unittest"
    # Run the executable
    COMMAND test_HuePlusPlus
    # Depends on test_HuePlusPlus
    DEPENDS test_HuePlusPlus
)

# Check for coverage test prerequisites
find_program( GCOV_PATH gcov )
find_program( LCOV_PATH lcov )

if(LCOV_PATH AND GCOV_PATH)
    # GCov
    include(CodeCoverage.cmake)
    add_executable(testcov_HuePlusPlus ${TEST_SOURCES} ${hueplusplus_SOURCES})
    target_link_libraries(testcov_HuePlusPlus gtest gmock)
    # prevent Main.cpp from defining main()
    target_compile_definitions(testcov_HuePlusPlus PUBLIC MAIN_CPP_NO_MAIN_FUNCTION)
    target_include_directories(testcov_HuePlusPlus PUBLIC ${GTest_INCLUDE_DIRS})
    target_include_directories(testcov_HuePlusPlus PUBLIC ${HuePlusPlus_INCLUDE_DIR})
    set_property(TARGET testcov_HuePlusPlus PROPERTY CXX_STANDARD 14)
    set_property(TARGET testcov_HuePlusPlus PROPERTY CXX_EXTENSIONS OFF)
    # this will be already done by APPEND_COVERAGE_COMPILER_FLAGS()
    #set_target_properties(
    #        testcov_HuePlusPlus PROPERTIES
    #        COMPILE_FLAGS "-O0 -g -fprofile-arcs -ftest-coverage"
    #)
    # Normally this would be -lgcov, but on mac only -Lgcov works
    #set_target_properties(
    #        testcov_HuePlusPlus PROPERTIES
    #        LINK_FLAGS "-O0 -g -Lgcov -fprofile-arcs -ftest-coverage"
    #)
    # exclude some special files we do not want to profile
    set(COVERAGE_EXCLUDES
        '/usr/*'           # unix
        '*/hueplusplus/build/*'
        '*/json*'
        '*/test/*'
        '*/v1/*'           # iOS
    )
    APPEND_COVERAGE_COMPILER_FLAGS()
    SETUP_TARGET_FOR_COVERAGE(
            NAME "coveragetest"
            EXECUTABLE testcov_HuePlusPlus
            DEPENDENCIES testcov_HuePlusPlus
    )
endif()
