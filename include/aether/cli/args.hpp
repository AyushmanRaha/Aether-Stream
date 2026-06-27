#pragma once

#include <aether/core/config.hpp>
#include <aether/core/expected.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string_view>

namespace aether::cli {

struct BenchOptions {
    bool help{false};
    std::uint64_t messages{1000000};
    std::size_t payload_size{64};
    std::size_t capacity{65536};
    std::filesystem::path output_dir{"benchmark-results"};
};

struct PubOptions {
    bool help{false};
    std::filesystem::path wal_path{"data/sample.wal"};
    std::uint64_t messages{1000};
    aether::byte_count_t wal_size_bytes{64ull * 1024ull * 1024ull};
    bool flush_on_commit{false};
};

struct SubOptions {
    bool help{false};
    std::filesystem::path wal_path{};
    std::uint64_t messages{10};
    std::uint64_t limit{10};
    std::size_t capacity{1024};
};

struct ReplayOptions {
    bool help{false};
    std::filesystem::path wal_path{"data/sample.wal"};
    std::uint64_t limit{0};
    std::size_t payload_preview_bytes{32};
};

struct InspectWalOptions {
    bool help{false};
    std::filesystem::path wal_path{"data/sample.wal"};
    std::uint64_t limit{0};
    bool verbose{false};
};

[[nodiscard]] Expected<BenchOptions> parse_bench_args(std::span<const char* const> args);
[[nodiscard]] Expected<PubOptions> parse_pub_args(std::span<const char* const> args);
[[nodiscard]] Expected<SubOptions> parse_sub_args(std::span<const char* const> args);
[[nodiscard]] Expected<ReplayOptions> parse_replay_args(std::span<const char* const> args);
[[nodiscard]] Expected<InspectWalOptions> parse_inspect_wal_args(std::span<const char* const> args);

[[nodiscard]] std::string_view bench_help() noexcept;
[[nodiscard]] std::string_view pub_help() noexcept;
[[nodiscard]] std::string_view sub_help() noexcept;
[[nodiscard]] std::string_view replay_help() noexcept;
[[nodiscard]] std::string_view inspect_wal_help() noexcept;

} // namespace aether::cli
