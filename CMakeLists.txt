cmake_minimum_required(VERSION 3.20...3.31)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(quids)

# Add module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Common settings
include(cmake/common_settings.cmake)

# Add directories
add_subdirectory(src)
add_subdirectory(tests)

# Enable testing
enable_testing()

# Installation
install(TARGETS quids_core
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp")
