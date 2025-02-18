# Core compiler settings

# Check compiler features
include(CheckCXXCompilerFlag)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check for C++20 support
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
        message(FATAL_ERROR "GCC version must be at least 10.0 for C++20 support")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
        message(FATAL_ERROR "Clang version must be at least 10.0 for C++20 support")
    endif()
elseif(MSVC)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.29)
        message(FATAL_ERROR "MSVC version must be at least 19.29 for C++20 support")
    endif()
endif()

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    
    # Add threading support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    
    # Release build optimizations 
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    endif()
endif()

# Enable LTO for release builds
if(USE_LTO AND CMAKE_BUILD_TYPE MATCHES "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LTO_SUPPORTED)
    if(LTO_SUPPORTED)
        message(STATUS "IPO/LTO enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "IPO/LTO not supported")
    endif()
endif()

# Position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable compile commands export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add test directories conditionally
if(BUILD_TESTS)
    enable_testing()
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/tests)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
    endif()
    if(BUILD_BENCHMARKS AND NOT EXISTS ${CMAKE_SOURCE_DIR}/benchmarks)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/benchmarks)
    endif()
endif()