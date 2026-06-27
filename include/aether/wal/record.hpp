#pragma once

#include <aether/core/expected.hpp>
#include <aether/core/status.hpp>
#include <aether/core/types.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace aether::wal {

inline constexpr std::uint32_t wal_record_magic =
    static_cast<std::uint32_t>('A') | (static_cast<std::uint32_t>('W') << 8U) |
    (static_cast<std::uint32_t>('A') << 16U) | (static_cast<std::uint32_t>('L') << 24U);
inline constexpr std::uint16_t wal_format_version = 1;
inline constexpr std::uint16_t wal_record_header_size = 40;

struct WalRecordHeader {
    std::uint32_t magic{wal_record_magic};
    std::uint16_t version{wal_format_version};
    std::uint16_t header_size{wal_record_header_size};
    std::uint32_t payload_size{};
    std::uint64_t sequence{};
    std::uint64_t timestamp_ns{};
    std::uint32_t checksum{};
    std::uint32_t flags{};
};

struct WalRecordView {
    WalRecordHeader header{};
    std::span<const std::byte> payload{};
    byte_count_t offset{};
};

namespace detail {

constexpr void write_u16_le(std::array<std::byte, wal_record_header_size>& bytes,
                            std::size_t offset, std::uint16_t value) noexcept {
    bytes[offset] = static_cast<std::byte>(value & 0xFFU);
    bytes[offset + 1U] = static_cast<std::byte>((value >> 8U) & 0xFFU);
}

constexpr void write_u32_le(std::array<std::byte, wal_record_header_size>& bytes,
                            std::size_t offset, std::uint32_t value) noexcept {
    for (std::size_t i = 0; i < 4U; ++i) {
        bytes[offset + i] = static_cast<std::byte>((value >> (i * 8U)) & 0xFFU);
    }
}

constexpr void write_u64_le(std::array<std::byte, wal_record_header_size>& bytes,
                            std::size_t offset, std::uint64_t value) noexcept {
    for (std::size_t i = 0; i < 8U; ++i) {
        bytes[offset + i] = static_cast<std::byte>((value >> (i * 8U)) & 0xFFU);
    }
}

[[nodiscard]] constexpr std::uint16_t read_u16_le(std::span<const std::byte> bytes,
                                                  std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(std::to_integer<std::uint16_t>(bytes[offset])) |
           static_cast<std::uint16_t>(std::to_integer<std::uint16_t>(bytes[offset + 1U]) << 8U);
}

[[nodiscard]] constexpr std::uint32_t read_u32_le(std::span<const std::byte> bytes,
                                                  std::size_t offset) noexcept {
    std::uint32_t value{};
    for (std::size_t i = 0; i < 4U; ++i) {
        value |= std::to_integer<std::uint32_t>(bytes[offset + i]) << (i * 8U);
    }
    return value;
}

[[nodiscard]] constexpr std::uint64_t read_u64_le(std::span<const std::byte> bytes,
                                                  std::size_t offset) noexcept {
    std::uint64_t value{};
    for (std::size_t i = 0; i < 8U; ++i) {
        value |= std::to_integer<std::uint64_t>(bytes[offset + i]) << (i * 8U);
    }
    return value;
}

} // namespace detail

[[nodiscard]] inline std::array<std::byte, wal_record_header_size>
serialize_header(WalRecordHeader header) noexcept {
    std::array<std::byte, wal_record_header_size> bytes{};
    detail::write_u32_le(bytes, 0, header.magic);
    detail::write_u16_le(bytes, 4, header.version);
    detail::write_u16_le(bytes, 6, header.header_size);
    detail::write_u32_le(bytes, 8, header.payload_size);
    // Bytes 12-15 are reserved for the v1 format and must remain deterministic zeros.
    detail::write_u64_le(bytes, 16, header.sequence);
    detail::write_u64_le(bytes, 24, header.timestamp_ns);
    detail::write_u32_le(bytes, 32, header.checksum);
    detail::write_u32_le(bytes, 36, header.flags);
    return bytes;
}

[[nodiscard]] inline Expected<WalRecordHeader>
deserialize_header(std::span<const std::byte> bytes) noexcept {
    if (bytes.size() < wal_record_header_size) {
        return Status{StatusCode::empty, "partial wal header"};
    }

    WalRecordHeader header{};
    header.magic = detail::read_u32_le(bytes, 0);
    header.version = detail::read_u16_le(bytes, 4);
    header.header_size = detail::read_u16_le(bytes, 6);
    header.payload_size = detail::read_u32_le(bytes, 8);
    header.sequence = detail::read_u64_le(bytes, 16);
    header.timestamp_ns = detail::read_u64_le(bytes, 24);
    header.checksum = detail::read_u32_le(bytes, 32);
    header.flags = detail::read_u32_le(bytes, 36);
    return header;
}

[[nodiscard]] constexpr byte_count_t record_total_size(const WalRecordHeader& header) noexcept {
    return static_cast<byte_count_t>(header.header_size) +
           static_cast<byte_count_t>(header.payload_size);
}

[[nodiscard]] inline bool is_zero_filled(std::span<const std::byte> bytes) noexcept {
    for (const std::byte value : bytes) {
        if (value != std::byte{0}) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] inline Status validate_header_shape(const WalRecordHeader& header) noexcept {
    if (header.magic != wal_record_magic) {
        return Status{StatusCode::corrupted_record, "invalid wal magic"};
    }
    if (header.version != wal_format_version) {
        return Status{StatusCode::corrupted_record, "unsupported wal version"};
    }
    if (header.header_size != wal_record_header_size) {
        return Status{StatusCode::corrupted_record, "invalid wal header size"};
    }
    if (header.payload_size > max_payload_size) {
        return Status{StatusCode::corrupted_record, "invalid wal payload size"};
    }
    return Status::ok();
}

} // namespace aether::wal
