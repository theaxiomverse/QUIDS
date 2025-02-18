# Test utilities

# Helper function to add a test executable
function(add_quids_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE quids GTest::GTest GTest::Main)
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    gtest_discover_tests(${name})
endfunction()