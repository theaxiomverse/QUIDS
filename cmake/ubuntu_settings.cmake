# Ubuntu specific settings

# Set vendor root path 
set(VENDOR_ROOT "/usr" CACHE PATH "Root directory for vendor libraries")

# Configure standard paths
list(APPEND CMAKE_PREFIX_PATH ${VENDOR_ROOT})
list(APPEND CMAKE_PREFIX_PATH "/usr/local")

# Library paths
set(LIB_PATHS
    ${VENDOR_ROOT}/lib
    ${VENDOR_ROOT}/lib/x86_64-linux-gnu  
    /usr/local/lib
)

# Add library search paths
list(APPEND CMAKE_LIBRARY_PATH ${LIB_PATHS})

# Include paths
set(INCLUDE_PATHS 
    ${VENDOR_ROOT}/include
    /usr/local/include
)

# Add include search paths
list(APPEND CMAKE_INCLUDE_PATH ${INCLUDE_PATHS})

# Configure pkg-config
set(ENV{PKG_CONFIG_PATH} "${VENDOR_ROOT}/lib/pkgconfig:${VENDOR_ROOT}/lib/x86_64-linux-gnu/pkgconfig:$ENV{PKG_CONFIG_PATH}")

# Support for optional components
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(MINIUPNPC miniupnpc)
    pkg_check_modules(NATPMP libnatpmp)
endif()

# Print configuration
message(STATUS "Using vendor root: ${VENDOR_ROOT}")
message(STATUS "Library paths:")
foreach(path ${CMAKE_LIBRARY_PATH})
    message(STATUS "  ${path}")
endforeach()
message(STATUS "Include paths:") 
foreach(path ${CMAKE_INCLUDE_PATH})
    message(STATUS "  ${path}")
endforeach()