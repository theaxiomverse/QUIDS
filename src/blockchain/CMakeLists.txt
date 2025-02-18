

# Core blockchain library
if(BUILD_SHARED_LIBS)
    add_library(blockchain SHARED
        AIBlock.cpp
        StandardTransaction.cpp
        Account.cpp
        AddressManager.cpp
        Block.cpp
        BlockProducer.cpp
        Chain.cpp
        Transaction.cpp
    )
else()
    add_library(blockchain STATIC
        AIBlock.cpp
        StandardTransaction.cpp
        Account.cpp
        AddressManager.cpp
        Block.cpp
        BlockProducer.cpp
        Chain.cpp
        Transaction.cpp
    )
endif()

# Link dependencies
target_link_libraries(blockchain
    PUBLIC
        crypto
    PRIVATE
        OpenSSL::Crypto
        fmt::fmt
        spdlog::spdlog
)

target_include_directories(blockchain
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENSSL_INCLUDE_DIR})

# Add to parent scope
set(BLOCKCHAIN_LIB blockchain PARENT_SCOPE) 