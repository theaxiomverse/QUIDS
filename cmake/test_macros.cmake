# Test helper macros

# Add a test executable
macro(quids_add_test test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name} 
        PRIVATE
        quids
        GTest::GTest 
        GTest::Main
    )
    target_include_directories(${test_name}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    gtest_discover_tests(${test_name})
endmacro()

# Add a benchmark executable 
macro(quids_add_benchmark bench_name)
    add_executable(${bench_name} ${ARGN})
    target_link_libraries(${bench_name}
        PRIVATE
        quids
        benchmark::benchmark
        benchmark::benchmark_main
    )
    target_include_directories(${bench_name}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR} 
    )
endmacro()