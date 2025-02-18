# Add custom module path first
list(APPEND CMAKE_MODULE_PATH 
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

# Core dependencies with version requirements
find_package(OpenMP REQUIRED)
find_package(OpenSSL 3.0 REQUIRED)
find_package(fmt 8.0 REQUIRED)
find_package(ZLIB 1.2 REQUIRED)
find_package(CURL 7.80 REQUIRED)
find_package(spdlog 1.9 REQUIRED)

find_package(EIGEN3 REQUIRED)

# Required third-party libraries
find_package(RocksDB REQUIRED)
find_package(GMP REQUIRED)
find_package(ZSTD REQUIRED)
find_package(BLAKE3 REQUIRED)

# Testing dependencies
if(BUILD_TESTS)
    find_package(GTest 1.11 REQUIRED)
    if(BUILD_BENCHMARKS)
        find_package(benchmark 1.6 REQUIRED)
    endif()
endif()

# Configure ccache if enabled
if(USE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    endif()
endif()

# OS-specific path configuration
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include(ubuntu_settings)
elseif(APPLE)
    include(macos_settings)
elseif(WIN32) 
    include(windows_settings)
endif()

# Additional path configuration
include(libpaths)

# Include build utils
include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)

# Optional dependencies
find_package(MiniUPnPc)
if(MINIUPNPC_FOUND) 
    set(HAVE_UPNP ON)
endif()

find_package(NATPMP)
if(NATPMP_FOUND)
    set(HAVE_NATPMP ON)
endif()

# Configure OpenMP
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Print dependency status
message(STATUS "Found dependencies:")
message(STATUS "  OpenMP: ${OpenMP_CXX_VERSION}")
message(STATUS "  OpenSSL: ${OPENSSL_VERSION}")
message(STATUS "  Eigen3: ${EIGEN3_VERSION_STRING}")
message(STATUS "  fmt: ${fmt_VERSION}")
message(STATUS "  spdlog: ${spdlog_VERSION}")
message(STATUS "  CURL: ${CURL_VERSION}")
message(STATUS "  ZLIB: ${ZLIB_VERSION_STRING}")