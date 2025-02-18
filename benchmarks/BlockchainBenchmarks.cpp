#include <benchmark/benchmark.h>
#include "blockchain/Block.hpp"

static void BM_BlockCreation(benchmark::State& state) {
    for (auto _ : state) {
        // Add benchmark implementation
    }
}
BENCHMARK(BM_BlockCreation);

BENCHMARK_MAIN();