# Crypto component
add_library(crypto STATIC
    falcon_signature.cpp
    QuantumHashFunction.cpp
    QuantumInterface.cpp
)

# Link against Eigen3::Eigen directly since we use Eigen headers
target_link_libraries(crypto
    PUBLIC
    $<BUILD_INTERFACE:project_includes>
    $<INSTALL_INTERFACE:quids::project_includes>
    quantum
    Eigen3::Eigen
    PRIVATE
    OpenSSL::Crypto
    ${BLAKE3_LIBRARY}
)

target_include_directories(crypto
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${BLAKE3_INCLUDE_DIR}
)