# Utility functions

# Set path hint variables based on vendor root
function(set_vendor_paths name)
    set(${name}_ROOT "${VENDOR_ROOT}" PARENT_SCOPE)
    set(${name}_INCLUDE_DIR "${VENDOR_ROOT}/include" PARENT_SCOPE)
    set(${name}_LIBRARY_DIR "${VENDOR_ROOT}/lib" PARENT_SCOPE) 
endfunction()

# Common find library helper
function(find_vendor_library name)
    find_library(${name}_LIBRARY
        NAMES ${ARGN}
        PATHS ${${name}_LIBRARY_DIR}
        REQUIRED
    )
    if(NOT ${name}_LIBRARY)
        message(FATAL_ERROR "${name} library not found")
    endif()
    set(${name}_LIBRARY ${${name}_LIBRARY} PARENT_SCOPE)
endfunction()