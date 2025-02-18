# Project version information
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# Add build version
string(TIMESTAMP BUILD_TIMESTAMP "%Y%m%d")
set(BUILD_VERSION "${PROJECT_VERSION}+${BUILD_TIMESTAMP}")

# Create version header directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/quids)

# Configure version header
configure_file(
    ${CMAKE_SOURCE_DIR}/include/version.hpp.in
    ${CMAKE_BINARY_DIR}/include/quids/version.hpp
    @ONLY)