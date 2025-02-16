find_path(MINIUPNPC_INCLUDE_DIR
    NAMES miniupnpc.h miniupnpc/miniupnpc.h
    PATHS
        /usr/local/include
        /usr/include
        /usr/local/Cellar/miniupnpc/*/include
    NO_DEFAULT_PATH
    REQUIRED
)

find_library(MINIUPNPC_LIBRARY
    NAMES miniupnpc
    PATHS
        /usr/local/lib
        /usr/lib
        /usr/local/Cellar/miniupnpc/*/lib
    NO_DEFAULT_PATH
    REQUIRED
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Miniupnpc
    REQUIRED_VARS
        MINIUPNPC_LIBRARY
        MINIUPNPC_INCLUDE_DIR
)

if(MINIUPNPC_FOUND AND NOT TARGET Miniupnpc::Miniupnpc)
    add_library(Miniupnpc::Miniupnpc UNKNOWN IMPORTED)
    set_target_properties(Miniupnpc::Miniupnpc PROPERTIES
        IMPORTED_LOCATION "${MINIUPNPC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MINIUPNPC_INCLUDE_DIR}"
    )
endif()