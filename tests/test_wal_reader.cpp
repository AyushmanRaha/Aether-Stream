#include <aether/wal/checksum.hpp>
#include <aether/wal/wal_reader.hpp>
#include <aether/wal/wal_writer.hpp>
#include <array>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

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
           (std::string{"aether_wal_reader_test_"} + std::to_string(::getpid()) + suffix);
}

std::vector<std::byte> bytes_from(std::string_view text) {
    std::vector<std::byte> bytes(text.size());
    std::memcpy(bytes.data(), text.data(), text.size());
    return bytes;
}

aether::MessageView message(std::span<const std::byte> payload, aether::message_flags_t flags = 0) {
    return aether::MessageView{
        aether::MessageHeader{0, 0, static_cast<aether::payload_size_t>(payload.size()), flags},
        payload};
}

bool payload_equals(std::span<const std::byte> payload, std::string_view text) {
    return payload.size() == text.size() &&
           std::memcmp(payload.data(), text.data(), text.size()) == 0;
}

void write_payload_record(const std::filesystem::path& path, std::span<const std::byte> payload) {
    auto writer_result = aether::wal::WalWriter::create(path, 256);
    auto writer = std::move(writer_result).value();
    static_cast<void>(writer.append(message(payload)));
    static_cast<void>(writer.flush());
}

} // namespace

int main() {
    int failures = 0;
    const auto path = unique_path(".wal");
    std::filesystem::remove(path);

    const std::array<std::string_view, 3> texts{"alpha", "bravo", "charlie"};
    std::array<std::vector<std::byte>, 3> payloads{bytes_from(texts[0]), bytes_from(texts[1]),
                                                   bytes_from(texts[2])};
    {
        auto writer_result = aether::wal::WalWriter::create(path, 4096);
        failures += check(writer_result.has_value(), "writer create should succeed");
        if (!writer_result.has_value()) {
            return 1;
        }
        auto writer = std::move(writer_result).value();
        for (std::size_t i = 0; i < payloads.size(); ++i) {
            failures += check(
                writer.append(message(payloads[i], static_cast<std::uint32_t>(0x10 + i))).is_ok(),
                "writer append should succeed");
        }
        failures += check(writer.flush().is_ok(), "writer flush should succeed");
    }

    auto reader_result = aether::wal::WalReader::open(path);
    failures += check(reader_result.has_value(), "reader open should succeed");
    if (!reader_result.has_value()) {
        return 1;
    }
    auto reader = std::move(reader_result).value();
    for (std::size_t i = 0; i < texts.size(); ++i) {
        auto record = reader.next();
        failures += check(record.has_value(), "next should return record");
        if (record.has_value()) {
            failures += check(record.value().header.sequence == i, "sequence should be ordered");
            failures += check(payload_equals(record.value().payload, texts[i]),
                              "payload should match original data");
            failures += check(record.value().header.flags == static_cast<std::uint32_t>(0x10 + i),
                              "flags should match original message flags");
        }
    }
    auto eof = reader.next();
    failures += check(!eof.has_value() && eof.status().code() == aether::StatusCode::empty,
                      "EOF after records should be empty");

    failures += check(reader.reset().is_ok(), "reset should succeed");
    auto first_again = reader.next();
    failures += check(first_again.has_value() && first_again.value().header.sequence == 0,
                      "reset should allow rereading from beginning");

    failures += check(reader.reset().is_ok(), "reset before replay should succeed");
    std::uint64_t visited = 0;
    failures += check(reader
                          .replay([&visited](const aether::wal::WalRecordView&) {
                              ++visited;
                              return aether::Status::ok();
                          })
                          .is_ok(),
                      "replay should return ok");
    failures += check(visited == texts.size(), "replay should visit all records");

    failures += check(reader.reset().is_ok(), "reset before failing replay should succeed");
    visited = 0;
    const auto replay_status = reader.replay([&visited](const aether::wal::WalRecordView&) {
        ++visited;
        return aether::Status{aether::StatusCode::invalid_argument, "visitor stop"};
    });
    failures += check(replay_status.code() == aether::StatusCode::invalid_argument,
                      "visitor error should stop replay");
    failures += check(visited == 1, "visitor error should stop after first record");

    const auto corrupt_path = unique_path("_corrupt.wal");
    std::filesystem::remove(corrupt_path);
    auto corrupt_payload = bytes_from("payload");
    write_payload_record(corrupt_path, corrupt_payload);
    {
        std::fstream file(corrupt_path, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(static_cast<std::streamoff>(aether::wal::wal_record_header_size));
        char flipped{};
        file.read(&flipped, 1);
        flipped ^= 0x01;
        file.seekp(static_cast<std::streamoff>(aether::wal::wal_record_header_size));
        file.write(&flipped, 1);
    }
    auto corrupt_reader_result = aether::wal::WalReader::open(corrupt_path);
    failures += check(corrupt_reader_result.has_value(), "corrupt reader open should succeed");
    if (corrupt_reader_result.has_value()) {
        auto corrupt_reader = std::move(corrupt_reader_result).value();
        auto record = corrupt_reader.next();
        failures += check(!record.has_value() &&
                              record.status().code() == aether::StatusCode::corrupted_record,
                          "corrupt checksum should return corrupted_record");
    }

    const auto partial_header_path = unique_path("_partial_header.wal");
    std::filesystem::remove(partial_header_path);
    {
        std::ofstream file(partial_header_path, std::ios::binary | std::ios::trunc);
        file.write("AWAL", 4);
    }
    auto partial_header_reader = aether::wal::WalReader::open(partial_header_path);
    failures += check(partial_header_reader.has_value(), "partial header reader should open");
    if (partial_header_reader.has_value()) {
        auto partial = std::move(partial_header_reader).value().next();
        failures +=
            check(!partial.has_value() && partial.status().code() == aether::StatusCode::empty &&
                      partial.status().detail() == "partial wal record",
                  "partial header should return empty with partial detail");
    }

    const auto partial_payload_path = unique_path("_partial_payload.wal");
    std::filesystem::remove(partial_payload_path);
    {
        aether::wal::WalRecordHeader header{};
        header.payload_size = 10;
        header.checksum = aether::wal::compute_record_checksum(header, {});
        const auto serialized = aether::wal::serialize_header(header);
        std::ofstream file(partial_payload_path, std::ios::binary | std::ios::trunc);
        file.write(reinterpret_cast<const char*>(serialized.data()), serialized.size());
        file.write("abc", 3);
    }
    auto partial_payload_reader = aether::wal::WalReader::open(partial_payload_path);
    failures += check(partial_payload_reader.has_value(), "partial payload reader should open");
    if (partial_payload_reader.has_value()) {
        auto partial = std::move(partial_payload_reader).value().next();
        failures +=
            check(!partial.has_value() && partial.status().code() == aether::StatusCode::empty &&
                      partial.status().detail() == "partial wal record",
                  "partial payload should return empty with partial detail");
    }

    std::filesystem::remove(path);
    std::filesystem::remove(corrupt_path);
    std::filesystem::remove(partial_header_path);
    std::filesystem::remove(partial_payload_path);

    if (failures != 0) {
        return 1;
    }
    std::cout << "wal reader test passed\n";
    return 0;
}
