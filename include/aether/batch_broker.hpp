#pragma once

#include <aether/broker.hpp>
#include <aether/core/config.hpp>
#include <aether/core/status.hpp>
#include <aether/metrics/counters.hpp>
#include <aether/metrics/snapshot.hpp>
#include <aether/spsc_ring_buffer.hpp>
#include <cstddef>
#include <span>
#include <type_traits>
#include <utility>

namespace aether {

// In-memory SPSC broker with batch-oriented APIs. Persistent/WAL batching is
// deliberately not included so the durability protocol can remain
// simple and independently reviewable.
template <typename T, std::size_t Capacity> class BatchBroker {
public:
    using value_type = T;

    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

    BatchBroker() noexcept = default;
    explicit BatchBroker(const QueueConfig& config) noexcept
        : config_status_(detail::validate_queue_config_for_capacity(config, Capacity)) {}

    BatchBroker(const BatchBroker&) = delete;
    BatchBroker& operator=(const BatchBroker&) = delete;
    BatchBroker(BatchBroker&&) = delete;
    BatchBroker& operator=(BatchBroker&&) = delete;

    [[nodiscard]] Status config_status() const noexcept {
        return config_status_;
    }
    [[nodiscard]] bool valid() const noexcept {
        return config_status_.is_ok();
    }
    [[nodiscard]] metrics::BrokerMetricsSnapshot metrics_snapshot() const noexcept {
        return counters_.snapshot();
    }
    [[nodiscard]] metrics::BrokerMetricsSnapshot snapshot() const noexcept {
        return metrics_snapshot();
    }
    void reset_metrics() noexcept {
        counters_.reset();
    }

    [[nodiscard]] Status
    try_publish(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires(std::is_copy_constructible_v<T>)
    {
        if (!valid()) {
            counters_.record_publish_failed_invalid();
            return config_status_;
        }
        if (!queue_.try_push(value)) {
            counters_.record_publish_failed_full();
            return Status{StatusCode::full, "batch broker queue is full"};
        }
        counters_.record_published();
        return Status::ok();
    }

    [[nodiscard]] Status try_publish(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires(std::is_move_constructible_v<T>)
    {
        if (!valid()) {
            counters_.record_publish_failed_invalid();
            return config_status_;
        }
        if (!queue_.try_push(std::move(value))) {
            counters_.record_publish_failed_full();
            return Status{StatusCode::full, "batch broker queue is full"};
        }
        counters_.record_published();
        return Status::ok();
    }

    [[nodiscard]] Status try_consume(T& out) noexcept(std::is_nothrow_move_assignable_v<T>)
        requires(std::is_move_assignable_v<T>)
    {
        if (!valid()) {
            counters_.record_consume_failed_invalid();
            return config_status_;
        }
        if (!queue_.try_pop(out)) {
            counters_.record_consume_failed_empty();
            return Status{StatusCode::empty, "batch broker queue is empty"};
        }
        counters_.record_consumed();
        return Status::ok();
    }

    [[nodiscard]] std::size_t
    try_publish_batch(std::span<const T> messages) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires(std::is_copy_constructible_v<T>)
    {
        if (messages.empty()) {
            return 0;
        }
        if (!valid()) {
            counters_.record_publish_failed_invalid();
            return 0;
        }
        std::size_t published = 0;
        for (const T& message : messages) {
            if (!queue_.try_push(message)) {
                counters_.record_publish_failed_full();
                break;
            }
            ++published;
            counters_.record_published();
        }
        return published;
    }

    [[nodiscard]] std::size_t
    try_consume_batch(std::span<T> out) noexcept(std::is_nothrow_move_assignable_v<T>)
        requires(std::is_move_assignable_v<T>)
    {
        if (out.empty()) {
            return 0;
        }
        if (!valid()) {
            counters_.record_consume_failed_invalid();
            return 0;
        }
        std::size_t consumed = 0;
        for (T& value : out) {
            if (!queue_.try_pop(value)) {
                break;
            }
            ++consumed;
            counters_.record_consumed();
        }
        if (consumed == 0) {
            counters_.record_consume_failed_empty();
        }
        return consumed;
    }

    [[nodiscard]] bool empty() const noexcept {
        return queue_.empty();
    }
    [[nodiscard]] bool full() const noexcept {
        return queue_.full();
    }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept {
        return Capacity;
    }
    [[nodiscard]] std::size_t size_approx() const noexcept {
        return queue_.size_approx();
    }

private:
    Status config_status_{Status::ok()};
    SpscRingBuffer<T, Capacity> queue_{};
    metrics::BrokerCounters counters_{};
};

} // namespace aether
