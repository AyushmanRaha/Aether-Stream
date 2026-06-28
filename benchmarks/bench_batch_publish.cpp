#include <aether/batch_broker.hpp>
#include <aether/broker.hpp>
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

template <std::size_t Capacity> void broker_single(benchmark::State& state) {
    std::uint64_t processed = 0;
    for (auto _ : state) {
        aether::Broker<Payload64, Capacity> broker;
        for (std::uint64_t i = 0; i < 1024; ++i) {
            Payload64 in{i, i};
            benchmark::DoNotOptimize(in);
            if (!broker.try_publish(in).is_ok()) {
                state.SkipWithError("publish failed");
                break;
            }
            Payload64 out{};
            if (!broker.try_consume(out).is_ok()) {
                state.SkipWithError("consume failed");
                break;
            }
            benchmark::DoNotOptimize(out);
            ++processed;
        }
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(processed));
}

template <std::size_t Capacity, std::size_t BatchSize> void batch_broker(benchmark::State& state) {
    std::uint64_t processed = 0;
    std::array<Payload64, BatchSize> in{};
    std::array<Payload64, BatchSize> out{};
    for (auto _ : state) {
        aether::BatchBroker<Payload64, Capacity> broker;
        for (std::uint64_t batch = 0; batch < 1024 / BatchSize; ++batch) {
            for (std::size_t i = 0; i < BatchSize; ++i) {
                in[i] = Payload64{batch * BatchSize + i, batch};
            }
            benchmark::DoNotOptimize(in);
            if (broker.try_publish_batch(in) != BatchSize) {
                state.SkipWithError("batch publish failed");
                break;
            }
            if (broker.try_consume_batch(out) != BatchSize) {
                state.SkipWithError("batch consume failed");
                break;
            }
            benchmark::DoNotOptimize(out);
            processed += BatchSize;
        }
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(processed));
}
} // namespace

BENCHMARK_TEMPLATE(broker_single, 1024)->Name("BM_BrokerSingle_64B_1024");
BENCHMARK_TEMPLATE(batch_broker, 1024, 8)->Name("BM_BatchBroker_Batch8_64B_1024");
BENCHMARK_TEMPLATE(batch_broker, 1024, 32)->Name("BM_BatchBroker_Batch32_64B_1024");
BENCHMARK_TEMPLATE(broker_single, 65536)->Name("BM_BrokerSingle_64B_65536");
BENCHMARK_TEMPLATE(batch_broker, 65536, 8)->Name("BM_BatchBroker_Batch8_64B_65536");
BENCHMARK_TEMPLATE(batch_broker, 65536, 32)->Name("BM_BatchBroker_Batch32_64B_65536");
BENCHMARK_MAIN();
