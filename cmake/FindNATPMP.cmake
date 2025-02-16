find_path(NATPMP_INCLUDE_DIR
    NAMES natpmp.h
    PATHS
        /usr/local/include
        /usr/include
        /usr/local/Cellar/libnatpmp/*/include
    NO_DEFAULT_PATH
    REQUIRED
)

find_library(NATPMP_LIBRARY
    NAMES natpmp
    PATHS
        /usr/local/lib
        /usr/lib
        /usr/local/Cellar/libnatpmp/*/lib
    NO_DEFAULT_PATH
    REQUIRED
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NATPMP
    REQUIRED_VARS
        NATPMP_LIBRARY
        NATPMP_INCLUDE_DIR
)

if(NATPMP_FOUND AND NOT TARGET NATPMP::NATPMP)
    add_library(NATPMP::NATPMP UNKNOWN IMPORTED)
    set_target_properties(NATPMP::NATPMP PROPERTIES
        IMPORTED_LOCATION "${NATPMP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${NATPMP_INCLUDE_DIR}"
    )
endif()