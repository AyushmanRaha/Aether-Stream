#include <aether/spsc_ring_buffer.hpp>
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

int test_ordered_transfer() {
    constexpr std::uint64_t message_count = 1'000'000;
    aether::SpscRingBuffer<std::uint64_t, 1024> queue;
    std::atomic<bool> failed{false};
    std::atomic<std::uint64_t> consumed{0};

    std::thread producer([&] {
        aether::utils::set_current_thread_name("spsc-prod");
        aether::utils::SpinWait spin;
        for (std::uint64_t value = 0;
             value < message_count && !failed.load(std::memory_order_relaxed);) {
            if (queue.try_push(value)) {
                ++value;
                spin.reset();
            } else {
                spin.wait();
            }
        }
    });

    std::thread consumer([&] {
        aether::utils::set_current_thread_name("spsc-cons");
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
                spin.wait();
            }
        }
    });

    producer.join();
    consumer.join();

    int failures = 0;
    failures += check(!failed.load(std::memory_order_relaxed), "ordered transfer should not fail");
    failures += check(consumed.load(std::memory_order_relaxed) == message_count,
                      "consumer should receive every message");
    failures += check(queue.empty(), "queue should be empty after ordered transfer");
    return failures;
}

int test_backpressure() {
    aether::SpscRingBuffer<int, 4> queue;
    int failures = 0;
    for (int value = 0; value < 4; ++value) {
        failures += check(queue.try_push(value), "fill push should succeed");
    }
    failures += check(queue.full(), "queue should be full");
    failures += check(!queue.try_push(4), "push into full queue should fail");

    int out = -1;
    failures += check(queue.try_pop(out), "pop after full should succeed");
    failures += check(out == 0, "first popped value should be FIFO");
    failures += check(queue.try_push(4), "push after freeing one slot should succeed");

    for (int expected = 1; expected <= 4; ++expected) {
        out = -1;
        failures += check(queue.try_pop(out), "drain pop should succeed");
        failures += check(out == expected, "drain order should remain FIFO");
    }
    failures += check(queue.empty(), "queue should be empty after backpressure test");
    return failures;
}

} // namespace

int main() {
    const int failures = test_ordered_transfer() + test_backpressure();
    if (failures != 0) {
        return 1;
    }
    std::cout << "spsc concurrent test passed\n";
    return 0;
}
