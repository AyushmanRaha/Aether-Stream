#pragma once

#include <aether/metrics/snapshot.hpp>
#include <atomic>
#include <cstdint>

namespace aether::metrics {

class BrokerCounters {
public:
    BrokerCounters() noexcept = default;
    BrokerCounters(const BrokerCounters&) = delete;
    BrokerCounters& operator=(const BrokerCounters&) = delete;
    BrokerCounters(BrokerCounters&&) = delete;
    BrokerCounters& operator=(BrokerCounters&&) = delete;

    void record_published() noexcept {
        increment(published_);
    }
    void record_consumed() noexcept {
        increment(consumed_);
    }
    void record_publish_failed_full() noexcept {
        increment(publish_failed_full_);
    }
    void record_consume_failed_empty() noexcept {
        increment(consume_failed_empty_);
    }
    void record_publish_failed_invalid() noexcept {
        increment(publish_failed_invalid_);
    }
    void record_consume_failed_invalid() noexcept {
        increment(consume_failed_invalid_);
    }
    void record_wal_write(std::uint64_t bytes) noexcept {
        increment(wal_records_written_);
        wal_bytes_written_.fetch_add(bytes, std::memory_order_relaxed);
    }
    void record_wal_flush() noexcept {
        increment(wal_flushes_);
    }
    void record_wal_failure() noexcept {
        increment(wal_failures_);
    }
    void record_recovered_record() noexcept {
        increment(recovered_records_);
    }
    void record_recovery_failure() noexcept {
        increment(recovery_failures_);
    }

    [[nodiscard]] BrokerMetricsSnapshot snapshot() const noexcept {
        return BrokerMetricsSnapshot{published_.load(std::memory_order_relaxed),
                                     consumed_.load(std::memory_order_relaxed),
                                     publish_failed_full_.load(std::memory_order_relaxed),
                                     consume_failed_empty_.load(std::memory_order_relaxed),
                                     publish_failed_invalid_.load(std::memory_order_relaxed),
                                     consume_failed_invalid_.load(std::memory_order_relaxed),
                                     wal_bytes_written_.load(std::memory_order_relaxed),
                                     wal_records_written_.load(std::memory_order_relaxed),
                                     wal_flushes_.load(std::memory_order_relaxed),
                                     wal_failures_.load(std::memory_order_relaxed),
                                     recovered_records_.load(std::memory_order_relaxed),
                                     recovery_failures_.load(std::memory_order_relaxed)};
    }

    void reset() noexcept {
        published_.store(0, std::memory_order_relaxed);
        consumed_.store(0, std::memory_order_relaxed);
        publish_failed_full_.store(0, std::memory_order_relaxed);
        consume_failed_empty_.store(0, std::memory_order_relaxed);
        publish_failed_invalid_.store(0, std::memory_order_relaxed);
        consume_failed_invalid_.store(0, std::memory_order_relaxed);
        wal_bytes_written_.store(0, std::memory_order_relaxed);
        wal_records_written_.store(0, std::memory_order_relaxed);
        wal_flushes_.store(0, std::memory_order_relaxed);
        wal_failures_.store(0, std::memory_order_relaxed);
        recovered_records_.store(0, std::memory_order_relaxed);
        recovery_failures_.store(0, std::memory_order_relaxed);
    }

private:
    static void increment(std::atomic<std::uint64_t>& counter) noexcept {
        counter.fetch_add(1, std::memory_order_relaxed);
    }

    std::atomic<std::uint64_t> published_{};
    std::atomic<std::uint64_t> consumed_{};
    std::atomic<std::uint64_t> publish_failed_full_{};
    std::atomic<std::uint64_t> consume_failed_empty_{};
    std::atomic<std::uint64_t> publish_failed_invalid_{};
    std::atomic<std::uint64_t> consume_failed_invalid_{};
    std::atomic<std::uint64_t> wal_bytes_written_{};
    std::atomic<std::uint64_t> wal_records_written_{};
    std::atomic<std::uint64_t> wal_flushes_{};
    std::atomic<std::uint64_t> wal_failures_{};
    std::atomic<std::uint64_t> recovered_records_{};
    std::atomic<std::uint64_t> recovery_failures_{};
};

} // namespace aether::metrics

namespace aether {
using BrokerCounters = metrics::BrokerCounters;
} // namespace aether
