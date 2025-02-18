cmake_minimum_required(VERSION 3.20)
project(QUIDS VERSION 1.0.0 LANGUAGES CXX)

# Include project settings
include(cmake/project_settings.cmake)

# Set macOS specific options
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS deployment version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# Binary output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Configure global flags
if(MSVC)
    # Windows-specific flags
    add_compile_options(/W4 /MP /bigobj)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
else()
    # Unix-like system flags
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
    if(APPLE)
        # macOS-specific flags
        add_compile_options(-Wno-deprecated-declarations)
        set(CMAKE_MACOSX_RPATH ON)
    endif()
endif()

# Generate compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add custom module path first
list(APPEND CMAKE_MODULE_PATH 
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

# Dependency handling is in dependencies.cmake - this must be included first
# to ensure all external dependencies are available
include(cmake/dependencies.cmake)

# Verify that Eigen was properly set up
if(NOT TARGET Eigen3::Eigen)
    message(FATAL_ERROR "Eigen target not created - check FetchEigen.cmake configuration")
endif()

# Create interface library for common includes
add_library(project_includes INTERFACE)
target_include_directories(project_includes 
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Add source directory - after all dependencies are configured
add_subdirectory(src)

# Installation rules
include(cmake/install_rules.cmake)
