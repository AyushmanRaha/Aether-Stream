#pragma once

#include <aether/detail/platform.hpp>
#include <cstdint>
#include <string_view>
#include <thread>

#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#include <pthread.h>
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#endif

namespace aether::utils {

inline void thread_yield() noexcept {
    std::this_thread::yield();
}

inline void cpu_relax() noexcept {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    _mm_pause();
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("yield" ::: "memory");
#else
    thread_yield();
#endif
#elif defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("" ::: "memory");
#else
    thread_yield();
#endif
}

class SpinWait {
public:
    explicit constexpr SpinWait(std::uint32_t pause_iterations = 64,
                                bool yield_after_spin = true) noexcept
        : pause_iterations_{pause_iterations}, yield_after_spin_{yield_after_spin} {}

    void wait() noexcept {
        if (counter_ < pause_iterations_) {
            ++counter_;
            cpu_relax();
            return;
        }

        if (yield_after_spin_) {
            thread_yield();
        } else {
            cpu_relax();
        }
    }

    void reset() noexcept {
        counter_ = 0;
    }

private:
    std::uint32_t counter_{};
    std::uint32_t pause_iterations_{};
    bool yield_after_spin_{};
};

inline void set_current_thread_name(std::string_view name) noexcept {
#if defined(__linux__)
    char buffer[16]{};
    const std::size_t count =
        name.size() < (sizeof(buffer) - 1) ? name.size() : (sizeof(buffer) - 1);
    for (std::size_t i = 0; i < count; ++i) {
        buffer[i] = name[i];
    }
    (void)pthread_setname_np(pthread_self(), buffer);
#elif defined(__APPLE__) && defined(__MACH__)
    char buffer[64]{};
    const std::size_t count =
        name.size() < (sizeof(buffer) - 1) ? name.size() : (sizeof(buffer) - 1);
    for (std::size_t i = 0; i < count; ++i) {
        buffer[i] = name[i];
    }
    (void)pthread_setname_np(buffer);
#else
    (void)name;
#endif
}

} // namespace aether::utils
