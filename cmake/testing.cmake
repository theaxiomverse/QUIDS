# Testing configuration
if(BUILD_TESTS)
    enable_testing()
    
    # Configure test options
    set(TEST_OUTPUT_ON_FAILURE ON)
    set(GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
    
    # Create test directory if it doesn't exist
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/tests)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
    endif()
    
    # Add test directory if CMakeLists.txt exists
    if(EXISTS ${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt)
        add_subdirectory(tests)
    endif()
    
    # Add benchmarks if enabled
    if(BUILD_BENCHMARKS)
        add_subdirectory(benchmarks)
    endif()
    
    # Configure sanitizers for debug builds
    if(USE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES "Debug")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            set(SANITIZER_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
            set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
            set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
        endif()
    endif()
endif()