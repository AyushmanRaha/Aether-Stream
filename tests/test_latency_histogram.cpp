#include <aether/metrics/latency_histogram.hpp>
#include <cmath>
#include <iostream>
namespace {
int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }
    return 0;
}
} // namespace
int main() {
    int failures = 0;
    aether::metrics::LatencyHistogram h;
    failures += check(h.empty() && h.count() == 0, "empty count");
    failures += check(h.min() == 0 && h.max() == 0 && h.p50() == 0 && h.p95() == 0 &&
                          h.p99() == 0 && h.p999() == 0 && h.mean() == 0.0,
                      "empty stats zero");
    h.record(123);
    failures += check(h.count() == 1 && h.min() == 123 && h.max() == 123, "single min max");
    failures += check(h.p50() == 123 && h.p95() == 123 && h.p99() == 123 && h.p999() == 123 &&
                          h.mean() == 123.0,
                      "single percentiles");
    h.clear();
    for (auto v : {10ULL, 20ULL, 30ULL, 40ULL, 50ULL})
        h.record(v);
    failures += check(h.min() == 10 && h.max() == 50 && h.mean() == 30.0, "ordered stats");
    failures += check(h.p50() == 30 && h.p95() == 50 && h.p99() == 50, "ordered percentiles");
    h.clear();
    for (auto v : {100ULL, 10ULL, 50ULL, 20ULL})
        h.record(v);
    failures +=
        check(h.percentile(50) == 20 && h.percentile(75) == 50, "unordered percentiles sort copy");
    failures += check(h.percentile(-10) == 10 && h.percentile(0) == 10 &&
                          h.percentile(100) == 100 && h.percentile(150) == 100,
                      "percentile clamping");
    h.clear();
    failures += check(h.empty() && h.count() == 0, "clear empties");
    h.reserve(100);
    failures += check(h.empty() && h.count() == 0, "reserve does not add samples");
    if (failures != 0)
        return 1;
    std::cout << "latency histogram test passed\n";
    return 0;
}
