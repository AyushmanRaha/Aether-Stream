#include <aether/message.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>

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

    aether::MessageHeader header{};
    aether::MessageView empty_message{};
    std::array<std::byte, 4> bytes{std::byte{0x1}, std::byte{0x2}, std::byte{0x3}, std::byte{0x4}};
    const aether::PayloadView payload{bytes};
    const aether::Expected<aether::MessageView> result =
        aether::make_message_view(7, 12345, payload, 0xA);

    failures += check(sizeof(aether::MessageHeader) == 24, "MessageHeader should be 24 bytes");
    failures += check(header.sequence == 0, "default sequence should be zero");
    failures += check(header.timestamp_ns == 0, "default timestamp should be zero");
    failures += check(header.payload_size == 0, "default payload size should be zero");
    failures += check(header.flags == 0, "default flags should be zero");
    failures += check(empty_message.header.payload_size == 0,
                      "default MessageView payload size should be zero");
    failures += check(empty_message.payload.empty(), "default MessageView payload should be empty");
    failures += check(payload.size() == 4, "PayloadView should report correct size");
    failures += check(result.has_value(), "make_message_view should succeed");

    if (result.has_value()) {
        const aether::MessageView message = result.value();
        failures += check(message.header.sequence == 7, "message sequence should be 7");
        failures +=
            check(message.header.timestamp_ns == 12345, "message timestamp should be 12345");
        failures += check(message.header.payload_size == 4, "message payload size should be 4");
        failures += check(message.header.flags == 0xA, "message flags should be 0xA");
        failures += check(message.payload.data() == bytes.data(),
                          "message payload should point at original data");
        failures +=
            check(aether::validate_message_view(message).is_ok(), "valid message should validate");

        aether::MessageView bad_message = message;
        bad_message.header.payload_size = 3;
        failures += check(aether::validate_message_view(bad_message).code() ==
                              aether::StatusCode::invalid_argument,
                          "payload size mismatch should return invalid_argument");
    }

    failures += check(aether::payload_size_fits(0), "zero payload size should fit");
    failures += check(aether::payload_size_fits(static_cast<std::size_t>(aether::max_payload_size)),
                      "max payload size should fit");

    if (failures != 0) {
        return 1;
    }

    std::cout << "message test passed\n";
    return 0;
}
