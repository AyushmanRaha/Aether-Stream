#include <aether/broker.hpp>
#include <aether/persistent_broker.hpp>
#include <array>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <type_traits>
#include <unistd.h>

namespace {

struct Payload64 {
    std::uint64_t sequence{};
    std::uint64_t timestamp_ns{};
    std::array<std::byte, 48> data{};
};
static_assert(sizeof(Payload64) == 64);
static_assert(std::is_trivially_copyable_v<Payload64>);

std::filesystem::path wal_path(const char* name) {
    return std::filesystem::temp_directory_path() /
           (std::string{"aether-bench-"} + std::to_string(::getpid()) + "-" + name + ".wal");
}

template <std::size_t Capacity> void benchmark_in_memory(benchmark::State& state) {
    constexpr std::uint64_t messages = 1024;
    std::uint64_t published = 0;
    std::uint64_t consumed = 0;
    for (auto _ : state) {
        aether::Broker<Payload64, Capacity> broker;
        for (std::uint64_t sequence = 0; sequence < messages; ++sequence) {
            Payload64 in{sequence, sequence};
            const auto pub = broker.try_publish(in);
            if (!pub.is_ok()) {
                state.SkipWithError(pub.message().data());
                break;
            }
            Payload64 out{};
            const auto con = broker.try_consume(out);
            if (!con.is_ok()) {
                state.SkipWithError(con.message().data());
                break;
            }
            benchmark::DoNotOptimize(out);
        }
        const auto metrics = broker.metrics_snapshot();
        published += metrics.published;
        consumed += metrics.consumed;
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(published));
    state.counters["published"] = static_cast<double>(published);
    state.counters["consumed"] = static_cast<double>(consumed);
}

template <std::size_t Capacity> void benchmark_wal(benchmark::State& state) {
    constexpr std::uint64_t messages = 1024;
    std::uint64_t published = 0;
    std::uint64_t consumed = 0;
    std::uint64_t wal_bytes = 0;
    std::uint64_t wal_records = 0;
    std::uint64_t file_index = 0;
    for (auto _ : state) {
        const auto path =
            wal_path((std::to_string(Capacity) + "-" + std::to_string(file_index++)).c_str());
        std::filesystem::remove(path);
        aether::WalConfig config{};
        config.path = path;
        config.file_size_bytes = 256ULL * 1024ULL * 1024ULL;
        config.flush_on_commit = false;
        aether::PersistentBroker<Payload64, Capacity> broker(config);
        if (!broker.valid()) {
            state.SkipWithError(broker.open_status().message().data());
            break;
        }
        for (std::uint64_t sequence = 0; sequence < messages; ++sequence) {
            Payload64 in{sequence, sequence};
            const auto pub = broker.try_publish(in);
            if (!pub.is_ok()) {
                state.SkipWithError(pub.message().data());
                break;
            }
            Payload64 out{};
            const auto con = broker.try_consume(out);
            if (!con.is_ok()) {
                state.SkipWithError(con.message().data());
                break;
            }
            benchmark::DoNotOptimize(out);
        }
        const auto metrics = broker.metrics_snapshot();
        published += metrics.published;
        consumed += metrics.consumed;
        wal_bytes += metrics.wal_bytes_written;
        wal_records += metrics.wal_records_written;
        std::filesystem::remove(path);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(published));
    state.counters["published"] = static_cast<double>(published);
    state.counters["consumed"] = static_cast<double>(consumed);
    state.counters["wal_bytes_written"] = static_cast<double>(wal_bytes);
    state.counters["wal_records_written"] = static_cast<double>(wal_records);
}

} // namespace

BENCHMARK_TEMPLATE(benchmark_in_memory, 1024)->Name("BM_BrokerEndToEnd_InMemory_64B_1024");
BENCHMARK_TEMPLATE(benchmark_in_memory, 65536)->Name("BM_BrokerEndToEnd_InMemory_64B_65536");
BENCHMARK_TEMPLATE(benchmark_wal, 1024)->Name("BM_BrokerEndToEnd_Wal_64B_1024");
BENCHMARK_TEMPLATE(benchmark_wal, 65536)->Name("BM_BrokerEndToEnd_Wal_64B_65536");

BENCHMARK_MAIN();
