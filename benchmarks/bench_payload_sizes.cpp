#include <aether/spsc_ring_buffer.hpp>
#include <aether/utils/clock.hpp>
#include <aether/utils/thread_utils.hpp>
#include <array>
#include <atomic>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <thread>

namespace {

template <std::size_t Bytes> struct Payload {
    static_assert(Bytes > sizeof(std::uint64_t));
    std::uint64_t sequence{};
    std::array<std::byte, Bytes - sizeof(std::uint64_t)> data{};
};

template <> struct Payload<sizeof(std::uint64_t)> {
    std::uint64_t sequence{};
};

static_assert(sizeof(Payload<8>) == 8);
static_assert(sizeof(Payload<32>) == 32);
static_assert(sizeof(Payload<64>) == 64);
static_assert(sizeof(Payload<256>) == 256);
static_assert(sizeof(Payload<1024>) == 1024);

struct PayloadStats {
    std::uint64_t checksum{};
    std::uint64_t producer_retries{};
    std::uint64_t consumer_retries{};
    bool order_valid{true};
};

template <std::size_t PayloadBytes, std::size_t Capacity>
PayloadStats run_payload_transfer(std::uint64_t messages) {
    aether::SpscRingBuffer<Payload<PayloadBytes>, Capacity> queue;
    std::atomic<std::uint64_t> producer_retries{0};
    std::atomic<std::uint64_t> consumer_retries{0};
    std::atomic<bool> order_valid{true};
    std::uint64_t checksum = 0;

    std::thread producer([&]() {
        aether::utils::set_current_thread_name("aeth_pay_prod");
        aether::utils::SpinWait spin;
        for (std::uint64_t sequence = 0; sequence < messages;) {
            Payload<PayloadBytes> payload{};
            payload.sequence = sequence;
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
        aether::utils::set_current_thread_name("aeth_pay_cons");
        aether::utils::SpinWait spin;
        Payload<PayloadBytes> payload{};
        for (std::uint64_t expected = 0; expected < messages;) {
            if (queue.try_pop(payload)) {
                if (payload.sequence != expected) {
                    order_valid.store(false, std::memory_order_relaxed);
                }
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

    return PayloadStats{checksum, producer_retries.load(std::memory_order_relaxed),
                        consumer_retries.load(std::memory_order_relaxed),
                        order_valid.load(std::memory_order_relaxed)};
}

template <std::size_t PayloadBytes, std::size_t Capacity>
void benchmark_payload_size(benchmark::State& state) {
    const auto messages = static_cast<std::uint64_t>(state.range(0));

    for (auto _ : state) {
        const aether::utils::Stopwatch stopwatch;
        const PayloadStats stats = run_payload_transfer<PayloadBytes, Capacity>(messages);
        const double elapsed_seconds = stopwatch.elapsed_seconds();
        state.SetIterationTime(elapsed_seconds);

        auto checksum = stats.checksum;
        benchmark::DoNotOptimize(checksum);
        benchmark::ClobberMemory();

        if (!stats.order_valid) {
            state.SkipWithError("SPSC payload-size benchmark observed out-of-order messages");
            break;
        }

        state.counters["producer_retries"] += static_cast<double>(stats.producer_retries);
        state.counters["consumer_retries"] += static_cast<double>(stats.consumer_retries);
    }

    const auto processed = state.iterations() * static_cast<std::int64_t>(messages);
    state.SetItemsProcessed(processed);
    state.SetBytesProcessed(processed * static_cast<std::int64_t>(PayloadBytes));
    state.counters["payload_size_bytes"] =
        benchmark::Counter(static_cast<double>(PayloadBytes), benchmark::Counter::kAvgThreads);
    state.counters["capacity"] =
        benchmark::Counter(static_cast<double>(Capacity), benchmark::Counter::kAvgThreads);
}

} // namespace

BENCHMARK_TEMPLATE(benchmark_payload_size, 8, 1024)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 32, 1024)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 64, 1024)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 256, 1024)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 1024, 1024)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 8, 65536)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 32, 65536)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 64, 65536)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 256, 65536)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();
BENCHMARK_TEMPLATE(benchmark_payload_size, 1024, 65536)
    ->Args({100000})
    ->Args({1000000})
    ->UseManualTime();

BENCHMARK_MAIN();
