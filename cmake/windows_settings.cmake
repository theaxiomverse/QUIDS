# Set Windows-specific paths and settings
set(ROCKSDB_ROOT "C:/Program Files/rocksdb")
set(ROCKSDB_INCLUDE_DIR "${ROCKSDB_ROOT}/include")
set(ROCKSDB_LIBRARY_DIR "${ROCKSDB_ROOT}/lib")

set(GMP_ROOT "C:/Program Files/gmp")
set(GMP_INCLUDE_DIR "${GMP_ROOT}/include")
set(GMP_LIBRARY_DIR "${GMP_ROOT}/lib")

set(BLAKE3_ROOT "C:/Program Files/BLAKE3")
set(BLAKE3_INCLUDE_DIR "${BLAKE3_ROOT}/include")
set(BLAKE3_LIBRARY_DIR "${BLAKE3_ROOT}/lib")

set(ZSTD_ROOT "C:/Program Files/zstd")
set(ZSTD_INCLUDE_DIR "${ZSTD_ROOT}/include")
set(ZSTD_LIBRARY_DIR "${ZSTD_ROOT}/lib")

# Add directories
include_directories(${ROCKSDB_INCLUDE_DIR})
include_directories(${GMP_INCLUDE_DIR})
include_directories(${BLAKE3_INCLUDE_DIR})
include_directories(${ZSTD_INCLUDE_DIR})

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

find_library(BLAKE3_LIBRARY
    NAMES blake3
    PATHS ${BLAKE3_LIBRARY_DIR}
    REQUIRED
)
if(NOT BLAKE3_LIBRARY)
    message(FATAL_ERROR "BLAKE3 library not found")
endif()

find_library(ZSTD_LIBRARY
    NAMES zstd
    PATHS ${ZSTD_LIBRARY_DIR}
    REQUIRED
)
if(NOT ZSTD_LIBRARY)
    message(FATAL_ERROR "ZSTD library not found")
endif()

# Windows-specific compiler and linker flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
