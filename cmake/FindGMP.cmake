find_path(GMP_INCLUDE_DIR
    NAMES gmp.h
    PATHS
        /usr/local/include
        /usr/include
        /usr/local/Cellar/gmp/*/include
    NO_DEFAULT_PATH
    REQUIRED
)

find_library(GMP_LIBRARY
    NAMES gmp
    PATHS
        /usr/local/lib
        /usr/lib
        /usr/local/Cellar/gmp/*/lib
    NO_DEFAULT_PATH
    REQUIRED
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP
    REQUIRED_VARS
        GMP_LIBRARY
        GMP_INCLUDE_DIR
)

if(GMP_FOUND AND NOT TARGET GMP::GMP)
    add_library(GMP::GMP UNKNOWN IMPORTED)
    set_target_properties(GMP::GMP PROPERTIES
        IMPORTED_LOCATION "${GMP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}"
    )
endif()