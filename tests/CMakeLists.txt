# Test files
file(GLOB_RECURSE TEST_SOURCES
    "tests/rollup/*.cpp"
    "tests/blockchain/*.cpp"
    "tests/quantum/*.cpp"
    "tests/zkp/*.cpp"
)

# Add test files
set(TEST_SOURCES
    ${TEST_SOURCES}
    blockchain/TransactionTest.cpp
    evm/EVMExecutorTest.cpp
    evm/uint256Test.cpp
)

# Create test executable
add_executable(enhanced_ml_tests ${TEST_SOURCES})
target_link_libraries(enhanced_ml_tests
    PRIVATE
    quids
    GTest::GTest
    GTest::Main
    fmt::fmt
    OpenSSL::Crypto
    OpenSSL::SSL
)

target_include_directories(enhanced_ml_tests
    PRIVATE
    ${JSON_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${VENDOR_DIR}
    ${FALCON_INCLUDE_DIR}
    ${SHA3_INCLUDE_DIR}
)

# Add test
add_test(
    NAME enhanced_ml_tests
    COMMAND enhanced_ml_tests
)

# Add separate high performance benchmark target
add_executable(rollup_benchmarks
    rollup/RollupBenchmarkTests.cpp
)

target_link_libraries(rollup_benchmarks
    PRIVATE
    quids
    GTest::GTest
    GTest::Main
    fmt::fmt
    OpenMP::OpenMP_CXX
    OpenSSL::Crypto
    OpenSSL::SSL
)

target_include_directories(rollup_benchmarks
    PRIVATE
    ${JSON_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR}
    ${VENDOR_DIR}
    ${FALCON_INCLUDE_DIR}
    ${SHA3_INCLUDE_DIR}
)

# Enable maximum optimization for benchmarks
target_compile_options(rollup_benchmarks PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-O3 -march=native -mtune=native>
    $<$<CXX_COMPILER_ID:Clang>:-O3 -march=native -mtune=native>
    $<$<CXX_COMPILER_ID:AppleClang>:-O3 -march=native -mtune=native>
)

if(ENABLE_LTO)
    set_property(TARGET rollup_benchmarks PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
