# FindRocksDB
# Finds the RocksDB library
#
# This will define:
# ROCKSDB_FOUND
# ROCKSDB_INCLUDE_DIR
# ROCKSDB_LIBRARY

find_path(ROCKSDB_INCLUDE_DIR
    NAMES rocksdb/db.h
    PATHS ${CMAKE_INCLUDE_PATH}
)

find_library(ROCKSDB_LIBRARY
    NAMES rocksdb
    PATHS ${CMAKE_LIBRARY_PATH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RocksDB
    DEFAULT_MSG
    ROCKSDB_LIBRARY
    ROCKSDB_INCLUDE_DIR
)