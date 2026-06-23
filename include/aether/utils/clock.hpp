#pragma once

#include <aether/core/types.hpp>
#include <chrono>
#include <cstdint>

namespace aether::utils {

[[nodiscard]] inline timestamp_ns_t monotonic_time_ns() noexcept {
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return static_cast<timestamp_ns_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());
}

class Stopwatch {
public:
    Stopwatch() noexcept : start_ns_{monotonic_time_ns()} {}

    void reset() noexcept {
        start_ns_ = monotonic_time_ns();
    }

    [[nodiscard]] timestamp_ns_t elapsed_ns() const noexcept {
        return monotonic_time_ns() - start_ns_;
    }

    [[nodiscard]] double elapsed_seconds() const noexcept {
        return static_cast<double>(elapsed_ns()) / 1'000'000'000.0;
    }

private:
    timestamp_ns_t start_ns_{};
};

} // namespace aether::utils
