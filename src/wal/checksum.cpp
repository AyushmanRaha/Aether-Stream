#include <aether/wal/checksum.hpp>
#include <array>
#include <cstddef>
#include <cstdint>

namespace aether::wal {

std::uint32_t crc32(std::span<const std::byte> bytes) noexcept {
    std::uint32_t crc = 0xFFFFFFFFU;
    for (const std::byte byte : bytes) {
        crc ^= std::to_integer<std::uint32_t>(byte);
        for (int bit = 0; bit < 8; ++bit) {
            const std::uint32_t mask = 0U - (crc & 1U);
            crc = (crc >> 1U) ^ (0xEDB88320U & mask);
        }
    }
    return crc ^ 0xFFFFFFFFU;
}

std::uint32_t compute_record_checksum(WalRecordHeader header,
                                      std::span<const std::byte> payload) noexcept {
    // The checksum field is zeroed while computing the record checksum so that the value is stable.
    header.checksum = 0;
    const auto serialized = serialize_header(header);

    std::uint32_t crc = 0xFFFFFFFFU;
    const auto update = [&crc](std::span<const std::byte> bytes) noexcept {
        for (const std::byte byte : bytes) {
            crc ^= std::to_integer<std::uint32_t>(byte);
            for (int bit = 0; bit < 8; ++bit) {
                const std::uint32_t mask = 0U - (crc & 1U);
                crc = (crc >> 1U) ^ (0xEDB88320U & mask);
            }
        }
    };

    update(serialized);
    update(payload);
    return crc ^ 0xFFFFFFFFU;
}

bool validate_record_checksum(const WalRecordHeader& header,
                              std::span<const std::byte> payload) noexcept {
    return header.checksum == compute_record_checksum(header, payload);
}

} // namespace aether::wal
