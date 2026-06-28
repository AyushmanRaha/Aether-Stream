#pragma once

#include <aether/core/types.hpp>
#include <cstdint>

namespace aether::metrics {

struct BrokerMetricsSnapshot {
    std::uint64_t published{};
    std::uint64_t consumed{};
    std::uint64_t publish_failed_full{};
    std::uint64_t consume_failed_empty{};
    std::uint64_t publish_failed_invalid{};
    std::uint64_t consume_failed_invalid{};
    std::uint64_t wal_bytes_written{};
    std::uint64_t wal_records_written{};
    std::uint64_t wal_flushes{};
    std::uint64_t wal_failures{};
    std::uint64_t recovered_records{};
    std::uint64_t recovery_failures{};
};

} // namespace aether::metrics

namespace aether {
using BrokerMetricsSnapshot = metrics::BrokerMetricsSnapshot;
} // namespace aether
