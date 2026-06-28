#include <aether/utils/spin_wait.hpp>
#include <benchmark/benchmark.h>
#include <thread>

namespace {
// Synthetic wait primitive overhead only; this does not imply end-to-end broker latency.
void BM_CpuRelax(benchmark::State& state) {
    for (auto _ : state) {
        aether::utils::cpu_relax();
    }
}
void BM_ThreadYield(benchmark::State& state) {
    for (auto _ : state) {
        std::this_thread::yield();
    }
}
void BM_SpinWaitPause(benchmark::State& state) {
    aether::utils::SpinWait wait{1024, false};
    for (auto _ : state) {
        wait.pause();
        benchmark::DoNotOptimize(wait.count());
    }
}
void BM_SpinWaitBackoff(benchmark::State& state) {
    for (auto _ : state) {
        aether::utils::SpinWait wait{4, true};
        for (int i = 0; i < 8; ++i) {
            wait.pause();
        }
        benchmark::DoNotOptimize(wait.count());
    }
}
} // namespace

BENCHMARK(BM_CpuRelax);
BENCHMARK(BM_ThreadYield);
BENCHMARK(BM_SpinWaitPause);
BENCHMARK(BM_SpinWaitBackoff);
BENCHMARK_MAIN();
