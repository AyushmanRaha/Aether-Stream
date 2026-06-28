#pragma once

#include <aether/core/config.hpp>
#include <atomic>
#include <cstdint>
#include <thread>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#endif

namespace aether::utils {

// Hint the current CPU that this thread is in a busy-wait loop. This can reduce
// pipeline pressure in tight polling loops, but still burns CPU and must be
// validated against the target deployment and workload.
inline void cpu_relax() noexcept {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    _mm_pause();
#elif defined(__aarch64__) || defined(__arm__) || defined(_M_ARM64)
#if defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("yield" ::: "memory");
#else
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
#else
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}

// Tiny deterministic backoff helper for hot polling paths. It never sleeps:
// early pauses use cpu_relax(), and later pauses may yield to the scheduler.
class SpinWait {
public:
    explicit constexpr SpinWait(
        std::uint32_t pause_iterations = QueueConfig{}.spin_pause_iterations,
        bool yield_after_spin = QueueConfig{}.yield_after_spin) noexcept
        : pause_iterations_(pause_iterations), yield_after_spin_(yield_after_spin) {}

    void pause() noexcept {
        if (count_ < pause_iterations_ || !yield_after_spin_) {
            cpu_relax();
        } else {
            std::this_thread::yield();
        }
        ++count_;
    }

    void reset() noexcept {
        count_ = 0;
    }
    [[nodiscard]] std::uint32_t count() const noexcept {
        return count_;
    }

private:
    std::uint32_t pause_iterations_{};
    bool yield_after_spin_{};
    std::uint32_t count_{};
};

} // namespace aether::utils
