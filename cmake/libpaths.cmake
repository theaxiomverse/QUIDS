# Library path configuration

# Add common library paths
list(APPEND CMAKE_LIBRARY_PATH
    /usr/lib
    /usr/local/lib
    /usr/lib/x86_64-linux-gnu
)

# Add vendor library paths
if(VENDOR_ROOT)
    list(APPEND CMAKE_LIBRARY_PATH
        ${VENDOR_ROOT}/lib
        ${VENDOR_ROOT}/lib64
    )
    list(APPEND CMAKE_INCLUDE_PATH
        ${VENDOR_ROOT}/include
    )
endif()

# Print paths
message(STATUS "Library search paths:")
foreach(path ${CMAKE_LIBRARY_PATH})
    message(STATUS "  ${path}")
endforeach()