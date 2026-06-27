#include <aether/wal/wal_writer.hpp>
#include <array>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>

namespace {
int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }
    return 0;
}

std::filesystem::path unique_path(const char* suffix) {
    return std::filesystem::temp_directory_path() /
           (std::string{"aether_wal_writer_test_"} + std::to_string(::getpid()) + suffix);
}

aether::MessageView message(std::span<const std::byte> payload, aether::message_flags_t flags = 0) {
    return aether::MessageView{
        aether::MessageHeader{0, 0, static_cast<aether::payload_size_t>(payload.size()), flags},
        payload};
}
} // namespace

int main() {
    int failures = 0;
    const auto path = unique_path(".wal");
    std::filesystem::remove(path);

    aether::WalConfig config{};
    config.path.clear();
    failures += check(!aether::wal::WalWriter::create(config).has_value(),
                      "invalid WalConfig path should fail");
    failures +=
        check(!aether::wal::WalWriter::create(path, 0).has_value(), "zero file size should fail");
    failures += check(
        !aether::wal::WalWriter::create(path, aether::wal::wal_record_header_size - 1).has_value(),
        "too-small file size should fail");

    auto writer_result = aether::wal::WalWriter::create(path, 256);
    failures += check(writer_result.has_value(), "creating normal WAL should succeed");
    if (!writer_result.has_value()) {
        return 1;
    }
    auto writer = std::move(writer_result).value();
    failures += check(writer.valid(), "writer should be valid");

    std::array<std::byte, 5> alpha{std::byte{'a'}, std::byte{'l'}, std::byte{'p'}, std::byte{'h'},
                                   std::byte{'a'}};
    failures += check(writer.append(message(alpha)).is_ok(), "append one record should succeed");
    failures += check(writer.current_offset() == aether::wal::wal_record_header_size + alpha.size(),
                      "current_offset should equal header plus payload");
    failures += check(writer.records_written() == 1, "records_written should increment");
    failures += check(writer.next_sequence() == 1, "next_sequence should increment from 0 to 1");
    failures += check(writer.remaining_space() == 256 - writer.current_offset(),
                      "remaining_space should decrease correctly");

    std::array<std::byte, 3> two{std::byte{'t'}, std::byte{'w'}, std::byte{'o'}};
    failures += check(writer.append(message(two)).is_ok(), "second append should succeed");
    failures += check(writer.append(message(two)).is_ok(), "third append should succeed");
    failures += check(writer.next_sequence() == 3, "multiple appends should increment sequence");

    std::array<std::byte, 200> too_big{};
    failures += check(writer.append(message(too_big)).code() == aether::StatusCode::out_of_space,
                      "out_of_space should be returned when record cannot fit");
    failures += check(writer.flush().is_ok(), "flush should succeed");

    auto zero_path = unique_path("_zero.wal");
    std::filesystem::remove(zero_path);
    auto zero_writer_result = aether::wal::WalWriter::create(zero_path, 128);
    failures += check(zero_writer_result.has_value(), "zero payload writer create should succeed");
    if (zero_writer_result.has_value()) {
        auto zero_writer = std::move(zero_writer_result).value();
        failures +=
            check(zero_writer.append(message({})).is_ok(), "zero-length payload append works");
        failures += check(zero_writer.current_offset() == aether::wal::wal_record_header_size,
                          "zero-length payload advances by header size");
    }

    std::filesystem::remove(path);
    std::filesystem::remove(zero_path);
    if (failures != 0) {
        return 1;
    }
    std::cout << "wal writer test passed\n";
    return 0;
}
