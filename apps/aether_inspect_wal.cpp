#include <aether/cli/args.hpp>
#include <aether/metrics/counters.hpp>
#include <aether/wal/record.hpp>
#include <aether/wal/wal_reader.hpp>
#include <iostream>
#include <limits>

namespace {
void print_error(aether::Status status) {
    std::cerr << "error: " << status.message() << '\n';
}
} // namespace

int main(int argc, char** argv) {
    const auto parsed = aether::cli::parse_inspect_wal_args({argv, static_cast<std::size_t>(argc)});
    if (!parsed) {
        print_error(parsed.status());
        return 2;
    }
    const auto options = parsed.value();
    if (options.help) {
        std::cout << aether::cli::inspect_wal_help();
        return 0;
    }
    std::cout << "WAL format: magic=0x" << std::hex << aether::wal::wal_record_magic << std::dec
              << " version=" << aether::wal::wal_format_version
              << " header_size=" << aether::wal::wal_record_header_size << '\n';
    auto reader_result = aether::wal::WalReader::open(options.wal_path);
    if (!reader_result) {
        print_error(reader_result.status());
        return 1;
    }
    auto reader = std::move(reader_result).value();
    aether::metrics::BrokerCounters counters;
    std::uint64_t count{};
    std::uint64_t total_payload{};
    std::uint64_t first_sequence = std::numeric_limits<std::uint64_t>::max();
    std::uint64_t last_sequence{};
    while (options.limit == 0 || count < options.limit) {
        auto record = reader.next();
        if (!record) {
            if (record.status().code() == aether::StatusCode::empty) {
                break;
            }
            counters.record_recovery_failure();
            std::cout << "scan ended with corruption/error at offset " << reader.current_offset()
                      << '\n';
            print_error(record.status());
            return 1;
        }
        const auto& view = record.value();
        if (count == 0) {
            first_sequence = view.header.sequence;
        }
        last_sequence = view.header.sequence;
        total_payload += view.header.payload_size;
        if (options.verbose) {
            std::cout << "record offset=" << view.offset << " sequence=" << view.header.sequence
                      << " payload_size=" << view.header.payload_size
                      << " timestamp_ns=" << view.header.timestamp_ns
                      << " flags=" << view.header.flags << " checksum=" << view.header.checksum
                      << '\n';
        }
        ++count;
        counters.record_recovered_record();
    }
    const auto metrics = counters.snapshot();
    std::cout << "record count: " << count << '\n'
              << "total payload bytes: " << total_payload << '\n'
              << "first sequence: " << (count == 0 ? 0 : first_sequence) << '\n'
              << "last sequence: " << (count == 0 ? 0 : last_sequence) << '\n'
              << "final reader offset: " << reader.current_offset() << '\n'
              << "scan ended cleanly: yes\n"
              << "metrics.recovered_records: " << metrics.recovered_records << '\n'
              << "metrics.recovery_failures: " << metrics.recovery_failures << '\n';
}
