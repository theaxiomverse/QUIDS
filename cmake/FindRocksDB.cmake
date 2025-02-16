find_path(ROCKSDB_INCLUDE_DIR
    NAMES rocksdb/db.h
    PATHS
        /usr/local/include
        /usr/include
        /usr/local/Cellar/rocksdb/*/include
    NO_DEFAULT_PATH
    REQUIRED
)

find_library(ROCKSDB_LIBRARY
    NAMES rocksdb
    PATHS
        /usr/local/lib
        /usr/lib
        /usr/local/Cellar/rocksdb/*/lib
    NO_DEFAULT_PATH
    REQUIRED
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RocksDB
    REQUIRED_VARS
        ROCKSDB_LIBRARY
        ROCKSDB_INCLUDE_DIR
)

if(ROCKSDB_FOUND AND NOT TARGET RocksDB::RocksDB)
    add_library(RocksDB::RocksDB UNKNOWN IMPORTED)
    set_target_properties(RocksDB::RocksDB PROPERTIES
        IMPORTED_LOCATION "${ROCKSDB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ROCKSDB_INCLUDE_DIR}"
    )
endif()