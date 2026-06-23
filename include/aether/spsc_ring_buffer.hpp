#pragma once

#include <aether/detail/cache_line.hpp>
#include <atomic>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace aether {

// Single Producer Single Consumer ring buffer.
//
// Exactly one producer thread may call try_push()/try_emplace(), and exactly one
// consumer thread may call try_pop(). Calling from multiple producers or
// multiple consumers is unsupported and would violate the ownership assumptions
// that make this implementation lock-free.
template <typename T, std::size_t Capacity> class SpscRingBuffer {
public:
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

    using value_type = T;

    SpscRingBuffer() noexcept = default;
    SpscRingBuffer(const SpscRingBuffer&) = delete;
    SpscRingBuffer& operator=(const SpscRingBuffer&) = delete;
    SpscRingBuffer(SpscRingBuffer&&) = delete;
    SpscRingBuffer& operator=(SpscRingBuffer&&) = delete;

    ~SpscRingBuffer() noexcept {
        destroy_remaining();
    }

    bool try_push(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        requires(std::is_copy_constructible_v<T>)
    {
        return try_emplace(value);
    }

    bool try_push(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires(std::is_move_constructible_v<T>)
    {
        return try_emplace(std::move(value));
    }

    template <typename... Args>
    bool try_emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        requires(std::is_constructible_v<T, Args...>)
    {
        const std::size_t head = head_.get().load(std::memory_order_relaxed);

        // The consumer release-stores tail_ after it has moved from and
        // destroyed a slot. This acquire load observes that release before the
        // producer decides the slot is free and constructs a new object there.
        const std::size_t tail = tail_.get().load(std::memory_order_acquire);

        if (head - tail >= Capacity) {
            return false;
        }

        std::construct_at(slot_ptr(head), std::forward<Args>(args)...);

        // Release-publish the constructed object. The consumer's acquire load
        // of head_ synchronizes with this store before it reads the slot.
        head_.get().store(head + 1, std::memory_order_release);
        return true;
    }

    bool try_pop(T& out) noexcept(std::is_nothrow_move_assignable_v<T>)
        requires(std::is_move_assignable_v<T>)
    {
        const std::size_t tail = tail_.get().load(std::memory_order_relaxed);

        // The producer release-stores head_ after constructing a slot. This
        // acquire load observes that release before the consumer moves from it.
        const std::size_t head = head_.get().load(std::memory_order_acquire);

        if (tail == head) {
            return false;
        }

        T* const slot = slot_ptr(tail);
        out = std::move(*slot);
        std::destroy_at(slot);

        // Release-publish that this slot's lifetime has ended. The producer's
        // acquire load of tail_ observes this before reusing the slot storage.
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
    struct Slot {
        alignas(T) unsigned char storage[sizeof(T)];
    };

    static constexpr std::size_t mask_ = Capacity - 1;

    [[nodiscard]] T* slot_ptr(std::size_t index) noexcept {
        return std::launder(reinterpret_cast<T*>(buffer_[index & mask_].storage));
    }

    void destroy_remaining() noexcept {
        std::size_t tail = tail_.get().load(std::memory_order_relaxed);
        const std::size_t head = head_.get().load(std::memory_order_relaxed);

        while (tail != head) {
            std::destroy_at(slot_ptr(tail));
            ++tail;
        }
    }

    Slot buffer_[Capacity]{};

    detail::CachePadded<std::atomic<std::size_t>> head_;
    detail::CachePadded<std::atomic<std::size_t>> tail_;
};

} // namespace aether
