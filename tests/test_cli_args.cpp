#include <aether/cli/args.hpp>
#include <cstdlib>
#include <iostream>

namespace {
void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "check failed: " << message << '\n';
        std::exit(1);
    }
}
template <std::size_t N> auto span_args(const char* const (&args)[N]) {
    return std::span<const char* const>{args, N};
}
template <typename T> void check_invalid(const T& result) {
    check(!result.has_value(), "expected parse failure");
    check(result.status().code() == aether::StatusCode::invalid_argument,
          "expected invalid_argument");
}
} // namespace

int main() {
    {
        const char* const args[] = {"aether_bench", "--help"};
        auto r = aether::cli::parse_bench_args(span_args(args));
        check(r && r.value().help, "bench help");
    }
    {
        const char* const args[] = {"aether_pub", "-h"};
        auto r = aether::cli::parse_pub_args(span_args(args));
        check(r && r.value().help, "pub short help");
    }
    {
        const char* const args[] = {"aether_sub", "--help"};
        auto r = aether::cli::parse_sub_args(span_args(args));
        check(r && r.value().help, "sub help");
    }
    {
        const char* const args[] = {"aether_replay", "-h"};
        auto r = aether::cli::parse_replay_args(span_args(args));
        check(r && r.value().help, "replay short help");
    }
    {
        const char* const args[] = {"aether_inspect_wal", "--help"};
        auto r = aether::cli::parse_inspect_wal_args(span_args(args));
        check(r && r.value().help, "inspect help");
    }

    {
        const char* const args[] = {"aether_bench"};
        auto r = aether::cli::parse_bench_args(span_args(args));
        check(r && r.value().messages == 1000000 && r.value().payload_size == 64 &&
                  r.value().capacity == 65536 && r.value().output_dir == "benchmark-results",
              "bench defaults");
    }
    {
        const char* const args[] = {"aether_pub"};
        auto r = aether::cli::parse_pub_args(span_args(args));
        check(r && r.value().wal_path == "data/sample.wal" && r.value().messages == 1000 &&
                  r.value().wal_size_bytes == 64ull * 1024ull * 1024ull &&
                  !r.value().flush_on_commit,
              "pub defaults");
    }
    {
        const char* const args[] = {"aether_sub"};
        auto r = aether::cli::parse_sub_args(span_args(args));
        check(r && r.value().messages == 10 && r.value().limit == 10 &&
                  r.value().capacity == 1024 && r.value().wal_path.empty(),
              "sub defaults");
    }
    {
        const char* const args[] = {"aether_replay"};
        auto r = aether::cli::parse_replay_args(span_args(args));
        check(r && r.value().wal_path == "data/sample.wal" && r.value().limit == 0 &&
                  r.value().payload_preview_bytes == 32,
              "replay defaults");
    }
    {
        const char* const args[] = {"aether_inspect_wal"};
        auto r = aether::cli::parse_inspect_wal_args(span_args(args));
        check(r && r.value().wal_path == "data/sample.wal" && r.value().limit == 0 &&
                  !r.value().verbose,
              "inspect defaults");
    }

    {
        const char* const args[] = {"aether_bench",       "--messages", "42",
                                    "--payload-size=256", "--capacity", "4096",
                                    "--output-dir=out"};
        auto r = aether::cli::parse_bench_args(span_args(args));
        check(r && r.value().messages == 42 && r.value().payload_size == 256 &&
                  r.value().capacity == 4096 && r.value().output_dir == "out",
              "bench values");
    }
    {
        const char* const args[] = {"aether_pub",       "--wal",      "x.wal",
                                    "--messages=11",    "--wal-size", "4096",
                                    "--flush-on-commit"};
        auto r = aether::cli::parse_pub_args(span_args(args));
        check(r && r.value().wal_path == "x.wal" && r.value().messages == 11 &&
                  r.value().wal_size_bytes == 4096 && r.value().flush_on_commit,
              "pub values");
    }
    {
        const char* const args[] = {"aether_sub",   "--wal=demo.wal", "--limit", "3",
                                    "--messages=9", "--capacity",     "64"};
        auto r = aether::cli::parse_sub_args(span_args(args));
        check(r && r.value().wal_path == "demo.wal" && r.value().limit == 3 &&
                  r.value().messages == 9 && r.value().capacity == 64,
              "sub values");
    }
    {
        const char* const args[] = {
            "aether_replay", "--payload-preview-bytes=8", "--limit", "2", "--wal", "r.wal"};
        auto r = aether::cli::parse_replay_args(span_args(args));
        check(r && r.value().payload_preview_bytes == 8 && r.value().limit == 2 &&
                  r.value().wal_path == "r.wal",
              "replay values");
    }
    {
        const char* const args[] = {"aether_inspect_wal", "--verbose", "--limit=5", "--wal",
                                    "i.wal"};
        auto r = aether::cli::parse_inspect_wal_args(span_args(args));
        check(r && r.value().verbose && r.value().limit == 5 && r.value().wal_path == "i.wal",
              "inspect values");
    }

    {
        const char* const args[] = {"aether_bench", "--unknown"};
        check_invalid(aether::cli::parse_bench_args(span_args(args)));
    }
    {
        const char* const args[] = {"aether_bench", "--messages"};
        check_invalid(aether::cli::parse_bench_args(span_args(args)));
    }
    {
        const char* const args[] = {"aether_bench", "--messages", "abc"};
        check_invalid(aether::cli::parse_bench_args(span_args(args)));
    }
    {
        const char* const args[] = {"aether_bench", "--messages", "-1"};
        check_invalid(aether::cli::parse_bench_args(span_args(args)));
    }
    {
        const char* const args[] = {"aether_bench", "--payload-size", "7"};
        check_invalid(aether::cli::parse_bench_args(span_args(args)));
    }
    {
        const char* const args[] = {"aether_sub", "--capacity", "123"};
        check_invalid(aether::cli::parse_sub_args(span_args(args)));
    }

    check(!aether::cli::bench_help().empty(), "bench help text");
    check(!aether::cli::pub_help().empty(), "pub help text");
    check(!aether::cli::sub_help().empty(), "sub help text");
    check(!aether::cli::replay_help().empty(), "replay help text");
    check(!aether::cli::inspect_wal_help().empty(), "inspect help text");
}
