#pragma once

#include <aether/core/expected.hpp>
#include <aether/core/types.hpp>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>

namespace aether {

using PayloadView = std::span<const std::byte>;

struct MessageHeader {
    sequence_t sequence{};
    timestamp_ns_t timestamp_ns{};
    payload_size_t payload_size{};
    message_flags_t flags{};
};

struct MessageView {
    MessageHeader header{};
    PayloadView payload{};
};

static_assert(std::is_trivially_copyable_v<MessageHeader>);
static_assert(sizeof(MessageHeader) == 24);

[[nodiscard]] constexpr bool payload_size_fits(std::size_t size) noexcept {
    return size <= max_payload_size;
}

[[nodiscard]] inline Expected<MessageView> make_message_view(sequence_t sequence,
                                                             timestamp_ns_t timestamp_ns,
                                                             PayloadView payload,
                                                             message_flags_t flags = 0) noexcept {
    if (!payload_size_fits(payload.size())) {
        return Status{StatusCode::invalid_argument, "payload too large"};
    }

    return MessageView{
        MessageHeader{sequence, timestamp_ns, static_cast<payload_size_t>(payload.size()), flags},
        payload};
}

[[nodiscard]] inline Status validate_message_view(const MessageView& message) noexcept {
    if (message.header.payload_size != message.payload.size()) {
        return Status{StatusCode::invalid_argument, "payload size mismatch"};
    }

    return Status::ok();
}

} // namespace aether
