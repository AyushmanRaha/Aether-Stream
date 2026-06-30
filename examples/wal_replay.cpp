// Writes sample WAL records and replays them to demonstrate WAL read/write behavior.
#include <aether/wal/wal_reader.hpp>
#include <aether/wal/wal_writer.hpp>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

std::vector<std::byte> bytes_from(std::string_view text) {
    std::vector<std::byte> bytes(text.size());
    std::memcpy(bytes.data(), text.data(), text.size());
    return bytes;
}

int print_error(std::string_view operation, aether::Status status) {
    std::cerr << operation << " failed: " << status.message() << '\n';
    return 1;
}

} // namespace

int main() {
    const std::filesystem::path data_dir{"data"};
    const std::filesystem::path wal_path = data_dir / "wal_replay.wal";
    std::filesystem::create_directories(data_dir);

    {
        auto writer_result = aether::wal::WalWriter::create(wal_path, 64U * 1024U);
        if (!writer_result.has_value()) {
            return print_error("create wal writer", writer_result.status());
        }

        auto writer = std::move(writer_result).value();
        const std::string_view samples[] = {"alpha", "bravo", "charlie"};
        for (std::string_view sample : samples) {
            auto payload = bytes_from(sample);
            const aether::MessageView message{
                aether::MessageHeader{0, 0, static_cast<aether::payload_size_t>(payload.size()), 0},
                payload};
            const aether::Status status = writer.append(message);
            if (!status.is_ok()) {
                return print_error("append wal record", status);
            }
        }

        const aether::Status flush_status = writer.flush();
        if (!flush_status.is_ok()) {
            return print_error("flush wal writer", flush_status);
        }
    }

    auto reader_result = aether::wal::WalReader::open(wal_path);
    if (!reader_result.has_value()) {
        return print_error("open wal reader", reader_result.status());
    }

    auto reader = std::move(reader_result).value();
    std::uint64_t count = 0;
    const aether::Status replay_status =
        reader.replay([&count](const aether::wal::WalRecordView& record) {
            const auto* text = reinterpret_cast<const char*>(record.payload.data());
            std::cout << "offset=" << record.offset << " sequence=" << record.header.sequence
                      << " timestamp_ns=" << record.header.timestamp_ns
                      << " payload_size=" << record.header.payload_size
                      << " flags=" << record.header.flags
                      << " payload=" << std::string_view{text, record.payload.size()} << '\n';
            ++count;
            return aether::Status::ok();
        });
    if (!replay_status.is_ok()) {
        return print_error("replay wal", replay_status);
    }

    std::cout << "records=" << count << '\n';
    return 0;
}
