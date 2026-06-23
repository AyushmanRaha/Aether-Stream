#pragma once

#include <aether/detail/cache_line.hpp>
#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>

namespace aether {

// Single Producer Single Consumer ring buffer.
//
// Exactly one producer thread may call try_push(), and exactly one consumer
// thread may call try_pop(). Calling try_push() from multiple producer threads
// or try_pop() from multiple consumer threads is unsupported and would violate
// the ownership assumptions that make this implementation lock-free.
template <typename T, std::size_t Capacity> class SpscRingBuffer {
public:
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
    static_assert(Capacity >= 2, "Capacity must be at least 2");
    static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
    static_assert(std::is_copy_assignable_v<T>, "T must be copy assignable");
    static_assert(std::is_nothrow_copy_assignable_v<T>,
                  "T must be nothrow copy assignable for noexcept push/pop");

    using value_type = T;

    SpscRingBuffer() noexcept = default;
    SpscRingBuffer(const SpscRingBuffer&) = delete;
    SpscRingBuffer& operator=(const SpscRingBuffer&) = delete;
    SpscRingBuffer(SpscRingBuffer&&) = delete;
    SpscRingBuffer& operator=(SpscRingBuffer&&) = delete;
    ~SpscRingBuffer() = default;

    bool try_push(const T& value) noexcept {
        const std::size_t head = head_.get().load(std::memory_order_relaxed);
        const std::size_t tail = tail_.get().load(std::memory_order_acquire);

        if (head - tail >= Capacity) {
            return false;
        }

        buffer_[head & mask_] = value;

        // Release-publish the written slot. The consumer's acquire load of
        // head_ observes this store before reading the slot contents.
        head_.get().store(head + 1, std::memory_order_release);
        return true;
    }

    bool try_pop(T& out) noexcept {
        const std::size_t tail = tail_.get().load(std::memory_order_relaxed);
        const std::size_t head = head_.get().load(std::memory_order_acquire);

        if (tail == head) {
            return false;
        }

        out = buffer_[tail & mask_];

        // Release-publish that the slot is available again. The producer's
        // acquire load of tail_ observes this before reusing the slot.
        tail_.get().store(tail + 1, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool empty() const noexcept {
        const std::size_t head = head_.get().load(std::memory_order_acquire);
        const std::size_t tail = tail_.get().load(std::memory_order_acquire);
        return head == tail;
    }

    [[nodiscard]] bool full() const noexcept {
        return size_approx() >= Capacity;
    }

    [[nodiscard]] static constexpr std::size_t capacity() noexcept {
        return Capacity;
    }

    [[nodiscard]] std::size_t size_approx() const noexcept {
        const std::size_t head = head_.get().load(std::memory_order_acquire);
        const std::size_t tail = tail_.get().load(std::memory_order_acquire);
        return head - tail;
    }

private:
    static constexpr std::size_t mask_ = Capacity - 1;

    std::array<T, Capacity> buffer_{};

    detail::CachePadded<std::atomic<std::size_t>> head_;
    detail::CachePadded<std::atomic<std::size_t>> tail_;
};

} // namespace aether
