#pragma once

#include <aether/wal/record.hpp>
#include <cstddef>
#include <cstdint>
#include <span>

namespace aether::wal {

[[nodiscard]] std::uint32_t crc32(std::span<const std::byte> bytes) noexcept;
[[nodiscard]] std::uint32_t compute_record_checksum(WalRecordHeader header,
                                                    std::span<const std::byte> payload) noexcept;
[[nodiscard]] bool validate_record_checksum(const WalRecordHeader& header,
                                            std::span<const std::byte> payload) noexcept;

} // namespace aether::wal
