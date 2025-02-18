# FindGMP - Find the GNU Multiple Precision Arithmetic Library
# Sets the following variables:
# GMP_FOUND - True if GMP was found
# GMP_INCLUDE_DIR - GMP include directory 
# GMP_LIBRARY - GMP library path

find_path(GMP_INCLUDE_DIR
    NAMES gmp.h
    PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(GMP_LIBRARY
    NAMES gmp
    PATHS ${CMAKE_LIBRARY_PATH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP 
    DEFAULT_MSG
    GMP_LIBRARY
    GMP_INCLUDE_DIR
)