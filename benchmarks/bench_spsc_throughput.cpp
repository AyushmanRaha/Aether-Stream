#include <aether/spsc_ring_buffer.hpp>
#include <aether/utils/clock.hpp>
#include <aether/utils/thread_utils.hpp>
#include <atomic>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <thread>

namespace {

struct ThroughputMessage {
    std::uint64_t sequence{};
};

struct TransferStats {
    std::uint64_t checksum{};
    std::uint64_t producer_retries{};
    std::uint64_t consumer_retries{};
    bool order_valid{true};
};

template <std::size_t Capacity> TransferStats run_transfer(std::uint64_t messages) {
    aether::SpscRingBuffer<ThroughputMessage, Capacity> queue;
    std::atomic<std::uint64_t> producer_retries{0};
    std::atomic<std::uint64_t> consumer_retries{0};
    std::atomic<bool> order_valid{true};
    std::uint64_t checksum = 0;

    std::thread producer([&]() {
        aether::utils::set_current_thread_name("aeth_bench_prod");
        aether::utils::SpinWait spin;
        for (std::uint64_t sequence = 0; sequence < messages;) {
            if (queue.try_push(ThroughputMessage{sequence})) {
                ++sequence;
                spin.reset();
            } else {
                producer_retries.fetch_add(1, std::memory_order_relaxed);
                spin.wait();
            }
        }
    });

    std::thread consumer([&]() {
        aether::utils::set_current_thread_name("aeth_bench_cons");
        aether::utils::SpinWait spin;
        ThroughputMessage message{};
        for (std::uint64_t expected = 0; expected < messages;) {
            if (queue.try_pop(message)) {
                if (message.sequence != expected) {
                    order_valid.store(false, std::memory_order_relaxed);
                }
                checksum += message.sequence;
                ++expected;
                spin.reset();
            } else {
                consumer_retries.fetch_add(1, std::memory_order_relaxed);
                spin.wait();
            }
        }
    });

    producer.join();
    consumer.join();

    return TransferStats{checksum, producer_retries.load(std::memory_order_relaxed),
                         consumer_retries.load(std::memory_order_relaxed),
                         order_valid.load(std::memory_order_relaxed)};
}

template <std::size_t Capacity> void benchmark_throughput(benchmark::State& state) {
    const auto messages = static_cast<std::uint64_t>(state.range(0));

    for (auto _ : state) {
        const aether::utils::Stopwatch stopwatch;
        const TransferStats stats = run_transfer<Capacity>(messages);
        const double elapsed_seconds = stopwatch.elapsed_seconds();
        state.SetIterationTime(elapsed_seconds);

        auto checksum = stats.checksum;
        benchmark::DoNotOptimize(checksum);
        benchmark::ClobberMemory();

        if (!stats.order_valid) {
            state.SkipWithError("SPSC throughput benchmark observed out-of-order messages");
            break;
        }

        state.counters["producer_retries"] += static_cast<double>(stats.producer_retries);
        state.counters["consumer_retries"] += static_cast<double>(stats.consumer_retries);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(messages));
    state.counters["messages_per_second"] = benchmark::Counter(
        static_cast<double>(state.iterations() * static_cast<std::int64_t>(messages)),
        benchmark::Counter::kIsRate);
    state.counters["capacity"] =
        benchmark::Counter(static_cast<double>(Capacity), benchmark::Counter::kAvgThreads);
}

} // namespace

BENCHMARK_TEMPLATE(benchmark_throughput, 64)->Args({100000})->Args({1000000})->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_throughput, 256)->Args({100000})->Args({1000000})->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_throughput, 1024)->Args({100000})->Args({1000000})->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_throughput, 65536)->Args({100000})->Args({1000000})->UseManualTime();

BENCHMARK_MAIN();
