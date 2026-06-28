#include <aether/spsc_ring_buffer.hpp>
#include <aether/zero_copy_spsc.hpp>
#include <array>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <type_traits>

namespace {
struct Payload64 {
    std::uint64_t sequence{};
    std::uint64_t timestamp_ns{};
    std::array<std::byte, 48> data{};

    Payload64() = default;
    Payload64(std::uint64_t seq, std::uint64_t ts) : sequence(seq), timestamp_ns(ts) {}
};
static_assert(sizeof(Payload64) == 64);
static_assert(std::is_trivially_copyable_v<Payload64>);

template <std::size_t Capacity> void spsc_emplace(benchmark::State& state) {
    aether::SpscRingBuffer<Payload64, Capacity> queue;
    Payload64 out{};
    std::uint64_t processed = 0;
    for (auto _ : state) {
        Payload64 in{processed, processed};
        benchmark::DoNotOptimize(in);
        if (!queue.try_emplace(in)) {
            state.SkipWithError("emplace failed");
            break;
        }
        if (!queue.try_pop(out)) {
            state.SkipWithError("pop failed");
            break;
        }
        benchmark::DoNotOptimize(out);
        ++processed;
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(processed));
}

template <std::size_t Capacity> void zero_copy(benchmark::State& state) {
    aether::ZeroCopySpsc<Payload64, Capacity> queue;
    Payload64 out{};
    std::uint64_t processed = 0;
    for (auto _ : state) {
        auto reservation = queue.try_reserve();
        if (!reservation) {
            state.SkipWithError("reserve failed");
            break;
        }
        benchmark::DoNotOptimize(reservation->construct(processed, processed));
        if (!reservation->commit()) {
            state.SkipWithError("commit failed");
            break;
        }
        if (!queue.try_consume(out)) {
            state.SkipWithError("consume failed");
            break;
        }
        benchmark::DoNotOptimize(out);
        ++processed;
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(processed));
}
} // namespace

BENCHMARK_TEMPLATE(spsc_emplace, 1024)->Name("BM_SpscTryEmplace_64B_1024");
BENCHMARK_TEMPLATE(zero_copy, 1024)->Name("BM_ZeroCopyReserveCommit_64B_1024");
BENCHMARK_TEMPLATE(spsc_emplace, 65536)->Name("BM_SpscTryEmplace_64B_65536");
BENCHMARK_TEMPLATE(zero_copy, 65536)->Name("BM_ZeroCopyReserveCommit_64B_65536");
BENCHMARK_MAIN();
