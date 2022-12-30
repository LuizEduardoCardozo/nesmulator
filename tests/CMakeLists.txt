project(Google_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# setup the test files
SET(
        TEST_FILES
        test_memory.cpp cpu_basic_functions_test.cpp)


# adding the Google_Tests_run target
add_executable(Google_Tests_run ${TEST_FILES})

# linking Google_Tests_run with DateConverter_lib which will be tested
target_link_libraries(Google_Tests_run ${CMAKE_PROJECT_NAME}_lib)

target_link_libraries(Google_Tests_run gtest gtest_main)