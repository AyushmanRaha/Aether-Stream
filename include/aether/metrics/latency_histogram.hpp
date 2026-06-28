#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace aether::metrics {

class LatencyHistogram {
public:
    using sample_t = std::uint64_t;

    LatencyHistogram() = default;
    explicit LatencyHistogram(std::size_t reserve_count);

    void reserve(std::size_t count);
    void record(sample_t latency_ns);
    void clear() noexcept;

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t count() const noexcept;
    [[nodiscard]] sample_t min() const noexcept;
    [[nodiscard]] sample_t max() const noexcept;
    [[nodiscard]] double mean() const noexcept;

    [[nodiscard]] sample_t percentile(double percentile_value) const;
    [[nodiscard]] sample_t p50() const;
    [[nodiscard]] sample_t p95() const;
    [[nodiscard]] sample_t p99() const;
    [[nodiscard]] sample_t p999() const;

private:
    std::vector<sample_t> samples_;
};

} // namespace aether::metrics

namespace aether {
using LatencyHistogram = metrics::LatencyHistogram;
} // namespace aether
