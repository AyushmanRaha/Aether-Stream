#include <aether/broker.hpp>
#include <aether/cli/args.hpp>
#include <aether/utils/clock.hpp>
#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

namespace {
template <std::size_t N> struct Payload {
    std::array<std::byte, N> bytes{};
};
struct Result {
    double elapsed_seconds{};
    double messages_per_second{};
    std::uint64_t full_retries{};
    std::uint64_t empty_retries{};
    aether::BrokerMetricsSnapshot metrics{};
};
void print_error(aether::Status status) {
    std::cerr << "error: " << status.message() << '\n';
}

template <std::size_t PayloadSize, std::size_t Capacity> Result run(std::uint64_t messages) {
    auto broker = std::make_unique<aether::Broker<Payload<PayloadSize>, Capacity>>();
    std::atomic<std::uint64_t> full_retries{};
    std::atomic<std::uint64_t> empty_retries{};
    aether::utils::Stopwatch timer{};
    std::thread producer([&] {
        for (std::uint64_t i = 0; i < messages;) {
            Payload<PayloadSize> payload{};
            payload.bytes[0] = static_cast<std::byte>(i & 0xffU);
            const auto status = broker->try_publish(payload);
            if (status) {
                ++i;
            } else if (status.code() == aether::StatusCode::full) {
                ++full_retries;
                std::this_thread::yield();
            }
        }
    });
    std::thread consumer([&] {
        Payload<PayloadSize> payload{};
        for (std::uint64_t i = 0; i < messages;) {
            const auto status = broker->try_consume(payload);
            if (status) {
                ++i;
            } else if (status.code() == aether::StatusCode::empty) {
                ++empty_retries;
                std::this_thread::yield();
            }
        }
    });
    producer.join();
    consumer.join();
    const double seconds = timer.elapsed_seconds();
    const auto metrics = broker->metrics_snapshot();
    return {seconds, static_cast<double>(messages) / seconds, full_retries.load(),
            empty_retries.load(), metrics};
}

template <std::size_t PayloadSize>
Result dispatch_capacity(std::size_t capacity, std::uint64_t messages) {
    switch (capacity) {
    case 64:
        return run<PayloadSize, 64>(messages);
    case 256:
        return run<PayloadSize, 256>(messages);
    case 1024:
        return run<PayloadSize, 1024>(messages);
    case 4096:
        return run<PayloadSize, 4096>(messages);
    case 65536:
        return run<PayloadSize, 65536>(messages);
    default:
        return {};
    }
}
Result dispatch(const aether::cli::BenchOptions& options) {
    switch (options.payload_size) {
    case 8:
        return dispatch_capacity<8>(options.capacity, options.messages);
    case 32:
        return dispatch_capacity<32>(options.capacity, options.messages);
    case 64:
        return dispatch_capacity<64>(options.capacity, options.messages);
    case 256:
        return dispatch_capacity<256>(options.capacity, options.messages);
    case 1024:
        return dispatch_capacity<1024>(options.capacity, options.messages);
    default:
        return {};
    }
}
std::filesystem::path result_path(const std::filesystem::path& dir) {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream name;
    name << "cli-bench-" << std::put_time(&tm, "%Y%m%d-%H%M%S") << ".txt";
    return dir / name.str();
}
} // namespace

int main(int argc, char** argv) {
    const auto parsed = aether::cli::parse_bench_args({argv, static_cast<std::size_t>(argc)});
    if (!parsed) {
        print_error(parsed.status());
        return 2;
    }
    const auto options = parsed.value();
    if (options.help) {
        std::cout << aether::cli::bench_help();
        return 0;
    }
    const auto result = dispatch(options);
    std::filesystem::create_directories(options.output_dir);
    const auto path = result_path(options.output_dir);
    std::ofstream out{path};
    auto write = [&](std::ostream& os) {
        os << "Aether-Stream CLI demo benchmark (not official performance data)\n"
           << "messages: " << options.messages << '\n'
           << "payload size: " << options.payload_size << '\n'
           << "capacity: " << options.capacity << '\n'
           << "elapsed seconds: " << result.elapsed_seconds << '\n'
           << "messages/sec: " << result.messages_per_second << '\n'
           << "producer full retries: " << result.full_retries << '\n'
           << "consumer empty retries: " << result.empty_retries << '\n'
           << "metrics.published: " << result.metrics.published << '\n'
           << "metrics.consumed: " << result.metrics.consumed << '\n'
           << "metrics.publish_failed_full: " << result.metrics.publish_failed_full << '\n'
           << "metrics.consume_failed_empty: " << result.metrics.consume_failed_empty << '\n';
    };
    write(std::cout);
    if (out) {
        write(out);
    }
    std::cout << "result file: " << path << '\n';
}
