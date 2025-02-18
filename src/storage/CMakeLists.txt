# Storage component
add_library(storage STATIC
    PersistentStorage.cpp
)

target_link_libraries(storage
    PRIVATE
    ${ROCKSDB_LIBRARY}
    ${ZSTD_LIBRARY}
)

target_include_directories(storage
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROCKSDB_INCLUDE_DIR}
    ${ZSTD_INCLUDE_DIR})