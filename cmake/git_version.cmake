# Get version from git

find_package(Git)
if(GIT_FOUND)
    # Get the current working branch
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    # Get the latest abbreviated commit hash
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

# Set version info
if(NOT GIT_BRANCH)
    set(GIT_BRANCH "unknown")
endif()

if(NOT GIT_COMMIT_HASH)
    set(GIT_COMMIT_HASH "unknown") 
endif()

set(VERSION_STRING "${PROJECT_VERSION}+${GIT_BRANCH}.${GIT_COMMIT_HASH}")
message(STATUS "Version: ${VERSION_STRING}")