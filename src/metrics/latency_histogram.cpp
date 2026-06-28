#include <aether/metrics/latency_histogram.hpp>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace aether::metrics {

LatencyHistogram::LatencyHistogram(std::size_t reserve_count) {
    reserve(reserve_count);
}
void LatencyHistogram::reserve(std::size_t count) {
    samples_.reserve(count);
}
void LatencyHistogram::record(sample_t latency_ns) {
    samples_.push_back(latency_ns);
}
void LatencyHistogram::clear() noexcept {
    samples_.clear();
}
bool LatencyHistogram::empty() const noexcept {
    return samples_.empty();
}
std::size_t LatencyHistogram::count() const noexcept {
    return samples_.size();
}
LatencyHistogram::sample_t LatencyHistogram::min() const noexcept {
    return samples_.empty() ? 0 : *std::min_element(samples_.begin(), samples_.end());
}
LatencyHistogram::sample_t LatencyHistogram::max() const noexcept {
    return samples_.empty() ? 0 : *std::max_element(samples_.begin(), samples_.end());
}
double LatencyHistogram::mean() const noexcept {
    if (samples_.empty())
        return 0.0;
    const auto total = std::accumulate(samples_.begin(), samples_.end(), 0.0);
    return total / static_cast<double>(samples_.size());
}
LatencyHistogram::sample_t LatencyHistogram::percentile(double percentile_value) const {
    if (samples_.empty())
        return 0;
    if (percentile_value <= 0.0)
        return min();
    if (percentile_value >= 100.0)
        return max();
    std::vector<sample_t> sorted = samples_;
    std::sort(sorted.begin(), sorted.end());
    const auto rank = static_cast<std::size_t>(
        std::ceil((percentile_value / 100.0) * static_cast<double>(sorted.size())));
    const auto index = (rank == 0 ? 1U : rank) - 1U;
    return sorted[index];
}
LatencyHistogram::sample_t LatencyHistogram::p50() const {
    return percentile(50.0);
}
LatencyHistogram::sample_t LatencyHistogram::p95() const {
    return percentile(95.0);
}
LatencyHistogram::sample_t LatencyHistogram::p99() const {
    return percentile(99.0);
}
LatencyHistogram::sample_t LatencyHistogram::p999() const {
    return percentile(99.9);
}

} // namespace aether::metrics
