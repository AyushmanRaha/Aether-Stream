#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace aether {

using sequence_t = std::uint64_t;
using timestamp_ns_t = std::uint64_t;
using byte_count_t = std::uint64_t;
using payload_size_t = std::uint32_t;
using message_flags_t = std::uint32_t;

inline constexpr sequence_t invalid_sequence = std::numeric_limits<sequence_t>::max();
inline constexpr payload_size_t max_payload_size = std::numeric_limits<payload_size_t>::max();
inline constexpr std::size_t default_queue_capacity = 1024;
inline constexpr byte_count_t default_wal_size_bytes = 64ull * 1024ull * 1024ull;

[[nodiscard]] constexpr bool is_power_of_two(std::size_t value) noexcept {
    return value != 0 && (value & (value - 1)) == 0;
}

} // namespace aether
