#include <aether/spsc_ring_buffer.hpp>
#include <aether/utils/clock.hpp>
#include <aether/utils/thread_utils.hpp>
#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>

namespace {

int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }
    return 0;
}

template <std::size_t Capacity> int run_case() {
    constexpr std::uint64_t message_count = 1'000'000;
    aether::SpscRingBuffer<std::uint64_t, Capacity> queue;
    std::atomic<bool> failed{false};
    std::atomic<std::uint64_t> consumed{0};
    std::atomic<std::uint64_t> push_retries{0};
    std::atomic<std::uint64_t> pop_retries{0};

    const aether::utils::Stopwatch stopwatch;

    std::thread producer([&] {
        aether::utils::SpinWait spin;
        for (std::uint64_t value = 0;
             value < message_count && !failed.load(std::memory_order_relaxed);) {
            if (queue.try_push(value)) {
                ++value;
                spin.reset();
            } else {
                push_retries.fetch_add(1, std::memory_order_relaxed);
                spin.wait();
            }
        }
    });

    std::thread consumer([&] {
        aether::utils::SpinWait spin;
        std::uint64_t expected = 0;
        while (expected < message_count && !failed.load(std::memory_order_relaxed)) {
            std::uint64_t value = 0;
            if (queue.try_pop(value)) {
                if (value != expected) {
                    failed.store(true, std::memory_order_relaxed);
                    break;
                }
                ++expected;
                consumed.store(expected, std::memory_order_relaxed);
                spin.reset();
            } else {
                pop_retries.fetch_add(1, std::memory_order_relaxed);
                spin.wait();
            }
        }
    });

    producer.join();
    consumer.join();

    const double seconds = stopwatch.elapsed_seconds();
    std::cout << "capacity=" << Capacity << " messages=" << message_count
              << " elapsed_seconds=" << seconds
              << " push_retries=" << push_retries.load(std::memory_order_relaxed)
              << " pop_retries=" << pop_retries.load(std::memory_order_relaxed) << '\n';

    int failures = 0;
    failures +=
        check(!failed.load(std::memory_order_relaxed), "stress transfer should preserve order");
    failures += check(consumed.load(std::memory_order_relaxed) == message_count,
                      "stress transfer should consume all messages");
    failures += check(queue.empty(), "stress queue should be empty after transfer");
    return failures;
}

} // namespace

int main() {
    const int failures = run_case<64>() + run_case<256>() + run_case<1024>() + run_case<65536>();
    if (failures != 0) {
        return 1;
    }
    std::cout << "spsc stress test passed\n";
    return 0;
}
