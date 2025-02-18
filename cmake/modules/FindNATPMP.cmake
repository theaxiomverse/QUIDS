# Find NAT-PMP library
# Sets:
# NATPMP_FOUND - System has NAT-PMP
# NATPMP_INCLUDE_DIR - NAT-PMP include directories
# NATPMP_LIBRARY - Library needed to use NAT-PMP

find_path(NATPMP_INCLUDE_DIR
    NAMES natpmp.h
    PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(NATPMP_LIBRARY
    NAMES natpmp
    PATHS ${CMAKE_LIBRARY_PATH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NATPMP
    REQUIRED_VARS
        NATPMP_LIBRARY
        NATPMP_INCLUDE_DIR
)