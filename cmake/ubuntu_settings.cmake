# Find BLAKE3 manually
find_path(BLAKE3_INCLUDE_DIR blake3.h
    PATHS /usr/include
    REQUIRED
)
find_library(BLAKE3_LIBRARY
    NAMES blake3
    PATHS /usr/lib
    REQUIRED
)

# Find zstd manually
find_path(ZSTD_INCLUDE_DIR zstd.h
    PATHS /usr/include
    REQUIRED
)
find_library(ZSTD_LIBRARY
    NAMES zstd
    PATHS /usr/lib
    REQUIRED
)

# Set RocksDB paths
set(ROCKSDB_ROOT "/usr")
set(ROCKSDB_INCLUDE_DIR "${ROCKSDB_ROOT}/include")
set(ROCKSDB_LIBRARY_DIR "${ROCKSDB_ROOT}/lib")

# Set GMP paths
set(GMP_ROOT "/usr")
set(GMP_INCLUDE_DIR "${GMP_ROOT}/include")
set(GMP_LIBRARY_DIR "${GMP_ROOT}/lib")

include_directories(${ROCKSDB_INCLUDE_DIR})
find_library(ROCKSDB_LIBRARY
    NAMES rocksdb
    PATHS ${ROCKSDB_LIBRARY_DIR}
    REQUIRED
)
if(NOT ROCKSDB_LIBRARY)
    message(FATAL_ERROR "RocksDB library not found")
endif()

find_library(GMP_LIBRARY
    NAMES gmp
    PATHS ${GMP_LIBRARY_DIR}
    REQUIRED
)
if(NOT GMP_LIBRARY)
    message(FATAL_ERROR "GMP library not found")
endif()
