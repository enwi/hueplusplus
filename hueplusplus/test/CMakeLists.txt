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

# get all test sources. Maybe use something else than GLOB
file(GLOB TEST_SOURCES test_*)

add_executable(test_HuePlusPlus ${TEST_SOURCES} ${hueplusplus_SOURCES})
target_link_libraries(test_HuePlusPlus gtest gmock)
# prevent Main.cpp from defining main()
target_compile_definitions(test_HuePlusPlus PUBLIC MAIN_CPP_NO_MAIN_FUNCTION)
target_include_directories(test_HuePlusPlus PUBLIC ${GTest_INCLUDE_DIRS})
target_include_directories(test_HuePlusPlus PUBLIC ${HuePlusPlus_INCLUDE_DIR})
set_property(TARGET test_HuePlusPlus PROPERTY CXX_STANDARD 14)
set_property(TARGET test_HuePlusPlus PROPERTY CXX_EXTENSIONS OFF)
add_custom_target("unittest"

    # make test_HuePlusPlus executable
    COMMAND "make" test_HuePlusPlus

    # Run the executable
    COMMAND "${CMAKE_BINARY_DIR}/hueplusplus/test/test_HuePlusPlus"
)

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
set(COVERAGE_EXCLUDES
    '/usr/*'           # unix
    '*/build/*'
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
