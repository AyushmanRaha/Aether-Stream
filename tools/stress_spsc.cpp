// Manual stress/validation tool for the SPSC ring buffer under sustained load.
#include <aether/spsc_ring_buffer.hpp>
#include <aether/utils/clock.hpp>
#include <aether/utils/thread_utils.hpp>
#include <atomic>
#include <charconv>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <thread>

namespace {

struct Options {
    std::uint64_t messages{10'000'000};
    std::size_t capacity{65536};
};

void print_help(const char* program) {
    std::cout << "Usage: " << program << " [--messages N] [--capacity 64|256|1024|4096|65536]\n";
}

bool parse_u64(std::string_view text, std::uint64_t& out) {
    const char* first = text.data();
    const char* last = text.data() + text.size();
    const auto result = std::from_chars(first, last, out);
    return result.ec == std::errc{} && result.ptr == last;
}

enum class ParseResult { ok, help, error };

ParseResult parse_args(int argc, char** argv, Options& options) {
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg{argv[i]};
        if (arg == "--help") {
            print_help(argv[0]);
            return ParseResult::help;
        }
        if (arg == "--messages" && i + 1 < argc) {
            if (!parse_u64(argv[++i], options.messages) || options.messages == 0) {
                std::cerr << "invalid --messages value\n";
                return ParseResult::error;
            }
            continue;
        }
        if (arg == "--capacity" && i + 1 < argc) {
            std::uint64_t parsed = 0;
            if (!parse_u64(argv[++i], parsed)) {
                std::cerr << "invalid --capacity value\n";
                return ParseResult::error;
            }
            options.capacity = static_cast<std::size_t>(parsed);
            continue;
        }
        std::cerr << "unknown or incomplete argument: " << arg << '\n';
        return ParseResult::error;
    }
    return ParseResult::ok;
}

template <std::size_t Capacity> bool run_stress(std::uint64_t messages) {
    aether::SpscRingBuffer<std::uint64_t, Capacity> queue;
    std::atomic<bool> failed{false};
    std::atomic<std::uint64_t> consumed{0};
    std::atomic<std::uint64_t> push_retries{0};
    std::atomic<std::uint64_t> pop_retries{0};

    const auto start_ns = aether::utils::monotonic_time_ns();

    std::thread producer([&] {
        aether::utils::set_current_thread_name("stress-prod");
        aether::utils::SpinWait spin;
        for (std::uint64_t value = 0;
             value < messages && !failed.load(std::memory_order_relaxed);) {
            if (queue.try_push(value)) {
                ++value;
                spin.reset();
            } else {
                push_retries.fetch_add(1, std::memory_order_relaxed);
                spin.wait();
            }
        }
    });

    std::thread consumer([&] {
        aether::utils::set_current_thread_name("stress-cons");
        aether::utils::SpinWait spin;
        std::uint64_t expected = 0;
        while (expected < messages && !failed.load(std::memory_order_relaxed)) {
            std::uint64_t value = 0;
            if (queue.try_pop(value)) {
                if (value != expected) {
                    failed.store(true, std::memory_order_relaxed);
                    break;
                }
                ++expected;
                consumed.store(expected, std::memory_order_relaxed);
                spin.reset();
            } else {
                pop_retries.fetch_add(1, std::memory_order_relaxed);
                spin.wait();
            }
        }
    });

    producer.join();
    consumer.join();

    const auto elapsed_ns = aether::utils::monotonic_time_ns() - start_ns;
    const double seconds = static_cast<double>(elapsed_ns) / 1'000'000'000.0;
    const bool passed = !failed.load(std::memory_order_relaxed) &&
                        consumed.load(std::memory_order_relaxed) == messages && queue.empty();

    std::cout << "capacity: " << Capacity << '\n'
              << "messages: " << messages << '\n'
              << "elapsed_seconds: " << seconds << '\n'
              << "messages_per_second: "
              << (seconds > 0.0 ? static_cast<double>(messages) / seconds : 0.0) << '\n'
              << "push_retries: " << push_retries.load(std::memory_order_relaxed) << '\n'
              << "pop_retries: " << pop_retries.load(std::memory_order_relaxed) << '\n'
              << "validation: " << (passed ? "passed" : "failed") << '\n';
    return passed;
}

} // namespace

int main(int argc, char** argv) {
    Options options;
    const ParseResult parse_result = parse_args(argc, argv, options);
    if (parse_result == ParseResult::help) {
        return 0;
    }
    if (parse_result == ParseResult::error) {
        return 1;
    }

    switch (options.capacity) {
    case 64:
        return run_stress<64>(options.messages) ? 0 : 1;
    case 256:
        return run_stress<256>(options.messages) ? 0 : 1;
    case 1024:
        return run_stress<1024>(options.messages) ? 0 : 1;
    case 4096:
        return run_stress<4096>(options.messages) ? 0 : 1;
    case 65536:
        return run_stress<65536>(options.messages) ? 0 : 1;
    default:
        std::cerr << "unsupported capacity: " << options.capacity << '\n';
        print_help(argv[0]);
        return 1;
    }
}
