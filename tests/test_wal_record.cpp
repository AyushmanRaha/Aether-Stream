#include <aether/wal/checksum.hpp>
#include <aether/wal/record.hpp>
#include <array>
#include <cstddef>
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

    static_assert(aether::wal::wal_record_header_size == 40);
    static_assert(sizeof(aether::wal::WalRecordHeader) == 40,
                  "The in-memory header is currently 40 bytes on supported targets; on-disk "
                  "serialization remains the authoritative fixed 40-byte format.");

    aether::wal::WalRecordHeader header{};
    header.payload_size = 5;
    header.sequence = 42;
    header.timestamp_ns = 1000;
    header.checksum = 0x12345678U;
    header.flags = 0xABCDEF00U;

    const auto bytes = aether::wal::serialize_header(header);
    failures += check(bytes.size() == 40, "serialized header should be 40 bytes");
    failures += check(bytes[0] == std::byte{'A'} && bytes[1] == std::byte{'W'} &&
                          bytes[2] == std::byte{'A'} && bytes[3] == std::byte{'L'},
                      "magic bytes should be AWAL");
    failures += check(aether::wal::wal_format_version == 1, "version should be 1");
    failures += check(bytes[12] == std::byte{0} && bytes[13] == std::byte{0} &&
                          bytes[14] == std::byte{0} && bytes[15] == std::byte{0},
                      "reserved bytes should serialize as zero");

    auto decoded = aether::wal::deserialize_header(bytes);
    failures += check(decoded.has_value(), "deserialize should succeed");
    if (decoded.has_value()) {
        failures += check(decoded.value().magic == header.magic, "magic should round-trip");
        failures += check(decoded.value().version == header.version, "version should round-trip");
        failures += check(decoded.value().header_size == header.header_size,
                          "header_size should round-trip");
        failures += check(decoded.value().payload_size == header.payload_size,
                          "payload_size should round-trip");
        failures +=
            check(decoded.value().sequence == header.sequence, "sequence should round-trip");
        failures += check(decoded.value().timestamp_ns == header.timestamp_ns,
                          "timestamp should round-trip");
        failures +=
            check(decoded.value().checksum == header.checksum, "checksum should round-trip");
        failures += check(decoded.value().flags == header.flags, "flags should round-trip");
    }

    auto bad = header;
    bad.magic = 1;
    failures += check(aether::wal::validate_header_shape(bad).code() ==
                          aether::StatusCode::corrupted_record,
                      "bad magic should fail validation");
    bad = header;
    bad.version = 2;
    failures += check(aether::wal::validate_header_shape(bad).code() ==
                          aether::StatusCode::corrupted_record,
                      "bad version should fail validation");
    bad = header;
    bad.header_size = 39;
    failures += check(aether::wal::validate_header_shape(bad).code() ==
                          aether::StatusCode::corrupted_record,
                      "bad header_size should fail validation");

    std::array<std::byte, 5> payload{std::byte{'h'}, std::byte{'e'}, std::byte{'l'}, std::byte{'l'},
                                     std::byte{'o'}};
    header.checksum = aether::wal::compute_record_checksum(header, payload);
    failures += check(aether::wal::validate_record_checksum(header, payload),
                      "checksum should validate for original payload");
    payload[0] = std::byte{'H'};
    failures += check(!aether::wal::validate_record_checksum(header, payload),
                      "checksum should fail after payload mutation");

    if (failures != 0) {
        return 1;
    }
    std::cout << "wal record test passed\n";
    return 0;
}
