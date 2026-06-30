// Demonstrates basic SPSC ring buffer publish/consume behavior.
#include <aether/message.hpp>
#include <aether/spsc_ring_buffer.hpp>
#include <array>
#include <iostream>

int main() {
    aether::SpscRingBuffer<int, 4> queue;

    if (!queue.try_push(1) || !queue.try_push(2) || !queue.try_push(3)) {
        std::cerr << "failed to push integer values\n";
        return 1;
    }

    for (const int expected : std::array{1, 2, 3}) {
        int value = 0;
        if (!queue.try_pop(value) || value != expected) {
            std::cerr << "unexpected integer queue result\n";
            return 1;
        }

        std::cout << "consumed: " << value << '\n';
    }

    aether::SpscRingBuffer<aether::MessageHeader, 4> headers;
    const aether::MessageHeader input_header{42, 1'000, 128, 0x1};
    aether::MessageHeader output_header{};

    if (!headers.try_push(input_header) || !headers.try_pop(output_header) ||
        output_header.sequence != input_header.sequence) {
        std::cerr << "unexpected MessageHeader queue result\n";
        return 1;
    }

    std::cout << "message sequence: " << output_header.sequence << '\n';
    return 0;
}
