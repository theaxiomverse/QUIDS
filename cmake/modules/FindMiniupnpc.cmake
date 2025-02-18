# Find miniupnpc library
# Sets:
# MINIUPNPC_FOUND - System has miniupnpc
# MINIUPNPC_INCLUDE_DIR - miniupnpc include directories
# MINIUPNPC_LIBRARY - Library needed to use miniupnpc

find_path(MINIUPNPC_INCLUDE_DIR
    NAMES miniupnpc/miniupnpc.h
    PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(MINIUPNPC_LIBRARY
    NAMES miniupnpc
    PATHS ${CMAKE_LIBRARY_PATH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Miniupnpc
    REQUIRED_VARS
        MINIUPNPC_LIBRARY
        MINIUPNPC_INCLUDE_DIR
)