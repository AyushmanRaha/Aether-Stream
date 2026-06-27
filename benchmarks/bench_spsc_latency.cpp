#include <aether/spsc_ring_buffer.hpp>
#include <aether/utils/clock.hpp>
#include <aether/utils/thread_utils.hpp>
#include <algorithm>
#include <atomic>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <thread>
#include <vector>

namespace {

struct TimestampedPayload {
    std::uint64_t sequence{};
    std::uint64_t timestamp_ns{};
    std::byte padding[48]{};
};

static_assert(sizeof(TimestampedPayload) == 64);

struct LatencyStats {
    std::uint64_t checksum{};
    std::uint64_t producer_retries{};
    std::uint64_t consumer_retries{};
    bool order_valid{true};
};

[[nodiscard]] std::uint64_t percentile_ns(const std::vector<std::uint64_t>& samples,
                                          double percentile) {
    if (samples.empty()) {
        return 0;
    }
    const auto index =
        static_cast<std::size_t>((percentile / 100.0) * static_cast<double>(samples.size() - 1));
    return samples[index];
}

template <std::size_t Capacity>
LatencyStats run_latency_transfer(std::uint64_t messages, std::vector<std::uint64_t>& latencies) {
    aether::SpscRingBuffer<TimestampedPayload, Capacity> queue;
    std::atomic<std::uint64_t> producer_retries{0};
    std::atomic<std::uint64_t> consumer_retries{0};
    std::atomic<bool> order_valid{true};
    std::uint64_t checksum = 0;

    std::thread producer([&]() {
        aether::utils::set_current_thread_name("aeth_lat_prod");
        aether::utils::SpinWait spin;
        for (std::uint64_t sequence = 0; sequence < messages;) {
            TimestampedPayload payload{};
            payload.sequence = sequence;
            payload.timestamp_ns = aether::utils::monotonic_time_ns();
            if (queue.try_push(payload)) {
                ++sequence;
                spin.reset();
            } else {
                producer_retries.fetch_add(1, std::memory_order_relaxed);
                spin.wait();
            }
        }
    });

    std::thread consumer([&]() {
        aether::utils::set_current_thread_name("aeth_lat_cons");
        aether::utils::SpinWait spin;
        TimestampedPayload payload{};
        for (std::uint64_t expected = 0; expected < messages;) {
            if (queue.try_pop(payload)) {
                const auto now_ns = aether::utils::monotonic_time_ns();
                if (payload.sequence != expected) {
                    order_valid.store(false, std::memory_order_relaxed);
                }
                latencies.push_back(now_ns - payload.timestamp_ns);
                checksum += payload.sequence;
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

    return LatencyStats{checksum, producer_retries.load(std::memory_order_relaxed),
                        consumer_retries.load(std::memory_order_relaxed),
                        order_valid.load(std::memory_order_relaxed)};
}

template <std::size_t Capacity> void benchmark_latency(benchmark::State& state) {
    const auto messages = static_cast<std::uint64_t>(state.range(0));

    for (auto _ : state) {
        std::vector<std::uint64_t> latencies;
        latencies.reserve(static_cast<std::size_t>(messages));

        const aether::utils::Stopwatch stopwatch;
        const LatencyStats stats = run_latency_transfer<Capacity>(messages, latencies);
        const double elapsed_seconds = stopwatch.elapsed_seconds();
        state.SetIterationTime(elapsed_seconds);

        auto checksum = stats.checksum;
        benchmark::DoNotOptimize(checksum);
        benchmark::ClobberMemory();

        if (!stats.order_valid || latencies.size() != messages) {
            state.SkipWithError("SPSC latency benchmark validation failed");
            break;
        }

        std::sort(latencies.begin(), latencies.end());
        state.counters["p50_ns"] = static_cast<double>(percentile_ns(latencies, 50.0));
        state.counters["p95_ns"] = static_cast<double>(percentile_ns(latencies, 95.0));
        state.counters["p99_ns"] = static_cast<double>(percentile_ns(latencies, 99.0));
        state.counters["p999_ns"] = static_cast<double>(percentile_ns(latencies, 99.9));
        state.counters["min_ns"] = static_cast<double>(latencies.front());
        state.counters["max_ns"] = static_cast<double>(latencies.back());
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

BENCHMARK_TEMPLATE(benchmark_latency, 1024)->Args({100000})->Args({200000})->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_latency, 65536)->Args({100000})->Args({200000})->UseManualTime();

BENCHMARK_MAIN();
