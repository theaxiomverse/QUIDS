# Find BLAKE3
# Sets the following variables:
# BLAKE3_FOUND - True if BLAKE3 was found
# BLAKE3_INCLUDE_DIR - BLAKE3 include directory
# BLAKE3_LIBRARY - BLAKE3 library

find_path(BLAKE3_INCLUDE_DIR
    NAMES blake3.h
    PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(BLAKE3_LIBRARY
    NAMES blake3
    PATHS ${CMAKE_LIBRARY_PATH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BLAKE3
    DEFAULT_MSG
    BLAKE3_LIBRARY
    BLAKE3_INCLUDE_DIR
)