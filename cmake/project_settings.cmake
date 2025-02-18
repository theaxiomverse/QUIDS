# Project-wide settings

# Policy settings
if(POLICY CMP0069)
    # Enable LTO/IPO when available
    cmake_policy(SET CMP0069 NEW)
endif()
if(POLICY CMP0077)
    # Allow options to override
    cmake_policy(SET CMP0077 NEW)
endif()

# Include core settings
include(${CMAKE_CURRENT_LIST_DIR}/options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/version.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/git_version.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/compiler_settings.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cpu_features.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/feature_checks.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/testing.cmake)

# Define vendor directory
set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendors)

# OS-specific settings
if(APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/macos_settings.cmake)
elseif(WIN32)
    include(${CMAKE_CURRENT_LIST_DIR}/windows_settings.cmake)
else()
    include(${CMAKE_CURRENT_LIST_DIR}/ubuntu_settings.cmake)
endif()

# Installation configuration will be handled in root CMakeLists.txt