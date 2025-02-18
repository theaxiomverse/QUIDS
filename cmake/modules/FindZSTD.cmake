# Find ZSTD
# Sets the following variables:
# ZSTD_FOUND - True if ZSTD was found
# ZSTD_INCLUDE_DIR - ZSTD include directory
# ZSTD_LIBRARY - ZSTD library

find_path(ZSTD_INCLUDE_DIR
    NAMES zstd.h
    PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(ZSTD_LIBRARY
    NAMES zstd
    PATHS ${CMAKE_LIBRARY_PATH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZSTD
    DEFAULT_MSG
    ZSTD_LIBRARY
    ZSTD_INCLUDE_DIR
)