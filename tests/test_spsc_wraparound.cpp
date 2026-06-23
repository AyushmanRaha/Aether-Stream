#include <aether/spsc_ring_buffer.hpp>
#include <array>
#include <cstdint>
#include <iostream>

namespace {

int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }

    return 0;
}

} // namespace

int main() {
    int failures = 0;

    aether::SpscRingBuffer<std::uint64_t, 8> queue;

    for (std::uint64_t cycle = 0; cycle < 1000; ++cycle) {
        for (std::uint64_t offset = 0; offset < 8; ++offset) {
            failures +=
                check(queue.try_push((cycle * 8) + offset), "fill/drain push should succeed");
        }

        for (std::uint64_t offset = 0; offset < 8; ++offset) {
            std::uint64_t out = 0;
            const std::uint64_t expected = (cycle * 8) + offset;
            failures += check(queue.try_pop(out), "fill/drain pop should succeed");
            failures += check(out == expected, "fill/drain order should remain FIFO");
        }
    }

    aether::SpscRingBuffer<std::uint64_t, 8> interleaved_queue;
    for (std::uint64_t value = 1; value <= 4; ++value) {
        failures +=
            check(interleaved_queue.try_push(value), "interleaved initial push should work");
    }

    for (std::uint64_t expected = 1; expected <= 2; ++expected) {
        std::uint64_t out = 0;
        failures += check(interleaved_queue.try_pop(out), "interleaved early pop should work");
        failures += check(out == expected, "interleaved early pop should be FIFO");
    }

    for (std::uint64_t value = 5; value <= 8; ++value) {
        failures += check(interleaved_queue.try_push(value), "interleaved wrap push should work");
    }

    for (const std::uint64_t expected : std::array<std::uint64_t, 6>{3, 4, 5, 6, 7, 8}) {
        std::uint64_t out = 0;
        failures += check(interleaved_queue.try_pop(out), "interleaved final pop should work");
        failures += check(out == expected, "interleaved final pop should be FIFO");
    }

    aether::SpscRingBuffer<std::uint64_t, 8> rolling_queue;
    for (std::uint64_t value = 0; value < 10000; ++value) {
        std::uint64_t out = 0;
        failures += check(rolling_queue.try_push(value), "rolling push should succeed");
        failures += check(rolling_queue.try_pop(out), "rolling pop should succeed");
        failures += check(out == value, "rolling pop should match pushed value");
    }

    if (failures != 0) {
        return 1;
    }

    std::cout << "spsc wraparound test passed\n";
    return 0;
}
