#include <aether/cli/args.hpp>
#include <aether/persistent_broker.hpp>
#include <filesystem>
#include <iostream>
#include <type_traits>

namespace {
struct OrderEvent {
    std::uint64_t id{};
    double price{};
    std::uint32_t quantity{};
};
static_assert(std::is_trivially_copyable_v<OrderEvent>);

OrderEvent make_event(std::uint64_t i) {
    return {i + 1U, 100.0 + static_cast<double>(i % 100U) * 0.25,
            1U + static_cast<std::uint32_t>(i % 1000U)};
}

void print_error(aether::Status status) {
    std::cerr << "error: " << status.message() << '\n';
}
} // namespace

int main(int argc, char** argv) {
    const auto parsed = aether::cli::parse_pub_args({argv, static_cast<std::size_t>(argc)});
    if (!parsed) {
        print_error(parsed.status());
        return 2;
    }
    const auto options = parsed.value();
    if (options.help) {
        std::cout << aether::cli::pub_help();
        return 0;
    }
    if (options.wal_path.has_parent_path()) {
        std::filesystem::create_directories(options.wal_path.parent_path());
    }
    aether::WalConfig config{};
    config.path = options.wal_path;
    config.file_size_bytes = options.wal_size_bytes;
    config.flush_on_commit = options.flush_on_commit;
    aether::PersistentBroker<OrderEvent, 65536> broker{};
    if (const auto status = broker.open(config); !status) {
        print_error(status);
        return 1;
    }
    for (std::uint64_t i = 0; i < options.messages; ++i) {
        if (const auto status = broker.try_publish(make_event(i)); !status) {
            print_error(status);
            return 1;
        }
    }
    if (const auto status = broker.flush(); !status) {
        print_error(status);
        return 1;
    }
    std::cout << "aether_pub wrote local demo OrderEvent records\n"
              << "wal path: " << options.wal_path << '\n'
              << "messages requested: " << options.messages << '\n'
              << "records written: " << broker.wal_records_written() << '\n'
              << "next sequence: " << broker.wal_next_sequence() << '\n'
              << "current WAL offset: " << broker.wal_current_offset() << '\n'
              << "remaining WAL space: " << broker.wal_remaining_space() << '\n';
}
