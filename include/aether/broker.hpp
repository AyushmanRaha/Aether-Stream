#pragma once

#include <aether/core/config.hpp>
#include <aether/core/status.hpp>
#include <aether/core/types.hpp>
#include <aether/spsc_ring_buffer.hpp>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <utility>

namespace aether {

enum class BrokerDurabilityMode : std::uint8_t { in_memory = 0, wal_before_queue = 1 };

[[nodiscard]] std::string_view broker_durability_mode_name(BrokerDurabilityMode mode) noexcept;

namespace detail {

[[nodiscard]] Status validate_queue_config_for_capacity(const QueueConfig& config,
                                                        std::size_t static_capacity) noexcept;

[[nodiscard]] Status validate_wal_config_for_payload(const WalConfig& config,
                                                     std::size_t payload_size) noexcept;

} // namespace detail

template <typename T, std::size_t Capacity> class Broker {
public:
    using value_type = T;

    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

    Broker() noexcept = default;

    explicit Broker(const QueueConfig& config) noexcept
        : config_status_(detail::validate_queue_config_for_capacity(config, Capacity)) {}

    Broker(const Broker&) = delete;
    Broker& operator=(const Broker&) = delete;
    Broker(Broker&&) = delete;
    Broker& operator=(Broker&&) = delete;

    [[nodiscard]] Status config_status() const noexcept {
        return config_status_;
    }

    [[nodiscard]] bool valid() const noexcept {
        return config_status_.is_ok();
    }

    [[nodiscard]] Status
    try_publish(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires(std::is_copy_constructible_v<T>)
    {
        if (!valid()) {
            return config_status_;
        }
        if (!queue_.try_push(value)) {
            return Status{StatusCode::full, "broker queue is full"};
        }
        return Status::ok();
    }

    [[nodiscard]] Status try_publish(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires(std::is_move_constructible_v<T>)
    {
        if (!valid()) {
            return config_status_;
        }
        if (!queue_.try_push(std::move(value))) {
            return Status{StatusCode::full, "broker queue is full"};
        }
        return Status::ok();
    }

    template <typename... Args>
    [[nodiscard]] Status
    try_emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        requires(std::is_constructible_v<T, Args...>)
    {
        if (!valid()) {
            return config_status_;
        }
        if (!queue_.try_emplace(std::forward<Args>(args)...)) {
            return Status{StatusCode::full, "broker queue is full"};
        }
        return Status::ok();
    }

    [[nodiscard]] Status try_consume(T& out) noexcept(std::is_nothrow_move_assignable_v<T>)
        requires(std::is_move_assignable_v<T>)
    {
        if (!valid()) {
            return config_status_;
        }
        if (!queue_.try_pop(out)) {
            return Status{StatusCode::empty, "broker queue is empty"};
        }
        return Status::ok();
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
    [[nodiscard]] static constexpr BrokerDurabilityMode durability_mode() noexcept {
        return BrokerDurabilityMode::in_memory;
    }

private:
    Status config_status_{Status::ok()};
    SpscRingBuffer<T, Capacity> queue_{};
};

} // namespace aether
