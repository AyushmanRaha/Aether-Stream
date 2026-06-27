#include <aether/cli/args.hpp>
#include <aether/wal/wal_reader.hpp>
#include <cctype>
#include <iostream>
#include <string>

namespace {
void print_error(aether::Status status) {
    std::cerr << "error: " << status.message() << '\n';
}
std::string preview(std::span<const std::byte> payload, std::size_t limit) {
    std::string out;
    const std::size_t count = std::min(payload.size(), limit);
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const auto ch = static_cast<unsigned char>(std::to_integer<unsigned int>(payload[i]));
        out.push_back(std::isprint(ch) != 0 ? static_cast<char>(ch) : '.');
    }
    return out;
}
} // namespace

int main(int argc, char** argv) {
    const auto parsed = aether::cli::parse_replay_args({argv, static_cast<std::size_t>(argc)});
    if (!parsed) {
        print_error(parsed.status());
        return 2;
    }
    const auto options = parsed.value();
    if (options.help) {
        std::cout << aether::cli::replay_help();
        return 0;
    }
    auto reader_result = aether::wal::WalReader::open(options.wal_path);
    if (!reader_result) {
        print_error(reader_result.status());
        return 1;
    }
    auto reader = std::move(reader_result).value();
    std::uint64_t records{};
    while (options.limit == 0 || records < options.limit) {
        auto record = reader.next();
        if (!record) {
            if (record.status().code() == aether::StatusCode::empty) {
                break;
            }
            print_error(record.status());
            return 1;
        }
        const auto& view = record.value();
        std::cout << "offset=" << view.offset << " sequence=" << view.header.sequence
                  << " timestamp_ns=" << view.header.timestamp_ns
                  << " payload_size=" << view.header.payload_size << " flags=" << view.header.flags
                  << " checksum=" << view.header.checksum << " preview=\""
                  << preview(view.payload, options.payload_preview_bytes) << "\"\n";
        ++records;
    }
    std::cout << "summary: records replayed=" << records
              << " final offset=" << reader.current_offset() << '\n';
}
