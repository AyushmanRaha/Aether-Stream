#include <aether/message.hpp>
#include <aether/spsc_ring_buffer.hpp>
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

    aether::SpscRingBuffer<int, 4> queue;
    int out = 0;

    failures += check(queue.empty(), "new queue should be empty");
    failures += check(!queue.full(), "new queue should not be full");
    failures += check(queue.capacity() == 4, "capacity should be exactly 4");
    failures += check(queue.size_approx() == 0, "initial size should be zero");
    failures += check(!queue.try_pop(out), "pop from empty queue should fail");
    failures += check(queue.try_push(42), "single push should succeed");
    failures += check(!queue.empty(), "queue should not be empty after push");
    failures += check(queue.try_pop(out), "single pop should succeed");
    failures += check(out == 42, "popped value should match pushed value");
    failures += check(queue.empty(), "queue should be empty after pop");

    for (int value = 0; value < 4; ++value) {
        failures += check(queue.try_push(value), "push while filling queue should succeed");
    }

    failures += check(queue.full(), "queue should report full after Capacity pushes");
    failures += check(!queue.try_push(99), "extra push should fail when full");

    for (int expected = 0; expected < 4; ++expected) {
        out = -1;
        failures += check(queue.try_pop(out), "pop while draining queue should succeed");
        failures += check(out == expected, "queue should preserve FIFO order");
    }

    failures += check(queue.empty(), "queue should be empty after draining");

    aether::SpscRingBuffer<int, 2> small_queue;
    failures += check(small_queue.try_push(7), "small queue first push should succeed");
    failures += check(small_queue.try_push(8), "small queue second push should succeed");
    failures += check(!small_queue.try_push(9), "small queue third push should fail");
    failures += check(small_queue.try_pop(out), "small queue first pop should succeed");
    failures += check(out == 7, "small queue first pop should be FIFO");
    failures += check(small_queue.try_pop(out), "small queue second pop should succeed");
    failures += check(out == 8, "small queue second pop should be FIFO");

    aether::SpscRingBuffer<aether::MessageHeader, 4> header_queue;
    const aether::MessageHeader header{123, 456, 789, 0xA};
    aether::MessageHeader popped_header{};
    failures += check(header_queue.try_push(header), "MessageHeader push should succeed");
    failures += check(header_queue.try_pop(popped_header), "MessageHeader pop should succeed");
    failures +=
        check(popped_header.sequence == header.sequence, "MessageHeader sequence should match");
    failures += check(popped_header.timestamp_ns == header.timestamp_ns,
                      "MessageHeader timestamp should match");
    failures += check(popped_header.payload_size == header.payload_size,
                      "MessageHeader payload size should match");
    failures += check(popped_header.flags == header.flags, "MessageHeader flags should match");

    if (failures != 0) {
        return 1;
    }

    std::cout << "spsc basic test passed\n";
    return 0;
}
