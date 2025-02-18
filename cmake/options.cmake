# Build options

option(BUILD_TESTS "Build test suite" ON)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
option(USE_SANITIZERS "Enable sanitizers in debug builds" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(USE_CCACHE "Use ccache if available" ON)

# Print build configuration
message(STATUS "")
message(STATUS "Build configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "  Use sanitizers: ${USE_SANITIZERS}")
message(STATUS "  Build shared libs: ${BUILD_SHARED_LIBS}")
message(STATUS "  Use ccache: ${USE_CCACHE}")
message(STATUS "")