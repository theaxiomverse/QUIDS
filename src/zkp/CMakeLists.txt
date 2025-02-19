# Zero-knowledge proof component
add_library(zkp STATIC
    QZKPGenerator.cpp
    QZKPVerifier.cpp
)

target_link_libraries(zkp
    PRIVATE
    crypto
    ${GMP_LIBRARY}
)

target_include_directories(zkp
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vendors/includes>
        "/usr/local/Cellar/blake3/1.5.5/include"
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GMP_INCLUDE_DIR})