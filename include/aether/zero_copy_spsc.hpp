#pragma once

#include <aether/detail/cache_line.hpp>
#include <atomic>
#include <cstddef>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

namespace aether {

// Experimental zero-copy SPSC queue. Exactly one producer may reserve/commit,
// and exactly one consumer may consume. A reservation must not outlive its queue;
// destroying the queue while a reservation is active is invalid usage.
template <typename T, std::size_t Capacity> class ZeroCopySpsc {
public:
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

    using value_type = T;

    class Reservation {
    public:
        Reservation() noexcept = default;
        Reservation(const Reservation&) = delete;
        Reservation& operator=(const Reservation&) = delete;

        Reservation(Reservation&& other) noexcept {
            move_from(other);
        }

        Reservation& operator=(Reservation&& other) noexcept {
            if (this != &other) {
                cancel();
                move_from(other);
            }
            return *this;
        }

        ~Reservation() noexcept {
            cancel();
        }

        template <typename... Args>
        T& construct(Args&&... args)
            requires(std::is_constructible_v<T, Args...>)
        {
            if (constructed_) {
                std::destroy_at(ptr_);
                constructed_ = false;
            }
            std::construct_at(ptr_, std::forward<Args>(args)...);
            constructed_ = true;
            return *ptr_;
        }

        [[nodiscard]] T* get() noexcept {
            return constructed_ ? ptr_ : nullptr;
        }
        [[nodiscard]] const T* get() const noexcept {
            return constructed_ ? ptr_ : nullptr;
        }
        [[nodiscard]] bool constructed() const noexcept {
            return constructed_;
        }

        bool commit() noexcept {
            if (!queue_ || !constructed_) {
                return false;
            }
            // Release-publish exactly one constructed slot. The consumer's
            // acquire load of head_ observes this before moving from the slot.
            queue_->head_.get().store(index_ + 1, std::memory_order_release);
            queue_->active_reservation_.get().store(false, std::memory_order_release);
            queue_ = nullptr;
            ptr_ = nullptr;
            constructed_ = false;
            return true;
        }

        void cancel() noexcept {
            if (!queue_) {
                return;
            }
            if (constructed_) {
                std::destroy_at(ptr_);
            }
            queue_->active_reservation_.get().store(false, std::memory_order_release);
            queue_ = nullptr;
            ptr_ = nullptr;
            constructed_ = false;
        }

    private:
        friend class ZeroCopySpsc;
        Reservation(ZeroCopySpsc* queue, std::size_t index, T* ptr) noexcept
            : queue_(queue), index_(index), ptr_(ptr) {}

        void move_from(Reservation& other) noexcept {
            queue_ = other.queue_;
            index_ = other.index_;
            ptr_ = other.ptr_;
            constructed_ = other.constructed_;
            other.queue_ = nullptr;
            other.ptr_ = nullptr;
            other.constructed_ = false;
        }

        ZeroCopySpsc* queue_{};
        std::size_t index_{};
        T* ptr_{};
        bool constructed_{};
    };

    ZeroCopySpsc() noexcept = default;
    ZeroCopySpsc(const ZeroCopySpsc&) = delete;
    ZeroCopySpsc& operator=(const ZeroCopySpsc&) = delete;
    ZeroCopySpsc(ZeroCopySpsc&&) = delete;
    ZeroCopySpsc& operator=(ZeroCopySpsc&&) = delete;

    ~ZeroCopySpsc() noexcept {
        destroy_remaining();
    }

    [[nodiscard]] std::optional<Reservation> try_reserve() noexcept {
        bool expected = false;
        if (!active_reservation_.get().compare_exchange_strong(
                expected, true, std::memory_order_acq_rel, std::memory_order_acquire)) {
            return std::nullopt;
        }

        const std::size_t head = head_.get().load(std::memory_order_relaxed);
        // The consumer release-stores tail_ after destroying a slot. This
        // acquire load observes that release before the producer reuses storage.
        const std::size_t tail = tail_.get().load(std::memory_order_acquire);
        if (head - tail >= Capacity) {
            active_reservation_.get().store(false, std::memory_order_release);
            return std::nullopt;
        }

        return Reservation{this, head, slot_ptr(head)};
    }

    [[nodiscard]] bool try_consume(T& out) noexcept(std::is_nothrow_move_assignable_v<T>)
        requires(std::is_move_assignable_v<T>)
    {
        const std::size_t tail = tail_.get().load(std::memory_order_relaxed);
        // The producer release-stores head_ only after commit. This acquire
        // load prevents the consumer from observing uncommitted data.
        const std::size_t head = head_.get().load(std::memory_order_acquire);
        if (tail == head) {
            return false;
        }
        T* const slot = slot_ptr(tail);
        out = std::move(*slot);
        std::destroy_at(slot);
        // Release-publish slot destruction before producer reuse.
        tail_.get().store(tail + 1, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool empty() const noexcept {
        return head_.get().load(std::memory_order_acquire) ==
               tail_.get().load(std::memory_order_acquire);
    }
    [[nodiscard]] bool full() const noexcept {
        return size_approx() >= Capacity;
    }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept {
        return Capacity;
    }
    [[nodiscard]] std::size_t size_approx() const noexcept {
        return head_.get().load(std::memory_order_acquire) -
               tail_.get().load(std::memory_order_acquire);
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
    detail::CachePadded<std::atomic<bool>> active_reservation_;
};

} // namespace aether
