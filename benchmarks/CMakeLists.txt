# Benchmark suite configuration

add_executable(quids_benchmarks
    BlockchainBenchmarks.cpp
    CryptoBenchmarks.cpp
    EVMBenchmarks.cpp
    RollupBenchmarks.cpp
)

target_link_libraries(quids_benchmarks
    PRIVATE
    quids
    benchmark::benchmark
    benchmark::benchmark_main
)

target_include_directories(quids_benchmarks
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR})