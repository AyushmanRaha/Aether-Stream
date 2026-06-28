#include <aether/batch_broker.hpp>
#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

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

    aether::BatchBroker<int, 8> broker;
    failures += check(broker.empty(), "new batch broker is empty");
    failures +=
        check(broker.metrics_snapshot().published == 0 && broker.metrics_snapshot().consumed == 0,
              "batch broker metrics start at zero");

    const std::array<int, 4> input{1, 2, 3, 4};
    failures +=
        check(broker.try_publish_batch(input) == input.size(), "batch publish accepts all items");
    std::array<int, 4> output{};
    failures +=
        check(broker.try_consume_batch(output) == output.size(), "batch consume returns all items");
    failures += check(output == input, "batch broker preserves order for one batch");

    aether::BatchBroker<int, 4> small;
    const std::array<int, 6> too_many{1, 2, 3, 4, 5, 6};
    failures += check(small.try_publish_batch(too_many) == 4, "partial publish fills capacity");
    failures += check(small.full(), "small batch broker reports full");
    failures += check(small.metrics_snapshot().publish_failed_full == 1,
                      "partial publish increments full once");
    const std::array<int, 2> extra{7, 8};
    failures += check(small.try_publish_batch(extra) == 0, "publish to full queue accepts zero");
    failures += check(small.metrics_snapshot().publish_failed_full == 2,
                      "full batch publish increments full once");
    std::array<int, 8> large_out{};
    failures +=
        check(small.try_consume_batch(large_out) == 4, "partial consume returns available count");
    failures += check(small.try_consume_batch(large_out) == 0, "empty consume batch returns zero");
    failures += check(small.metrics_snapshot().consume_failed_empty == 1,
                      "empty consume batch increments empty once");

    small.reset_metrics();
    std::array<int, 0> empty{};
    failures += check(small.try_publish_batch(empty) == 0, "empty publish batch returns zero");
    failures += check(small.try_consume_batch(empty) == 0, "empty consume batch returns zero");
    failures += check(small.metrics_snapshot().publish_failed_full == 0 &&
                          small.metrics_snapshot().consume_failed_empty == 0,
                      "empty spans do not increment failure metrics");

    aether::QueueConfig config{};
    config.capacity = 8;
    aether::BatchBroker<int, 4> invalid(config);
    failures += check(!invalid.valid(), "mismatched batch broker config is invalid");
    failures += check(invalid.try_publish_batch(input) == 0, "invalid batch publish returns zero");
    failures += check(invalid.try_consume_batch(output) == 0, "invalid batch consume returns zero");
    failures += check(invalid.metrics_snapshot().publish_failed_invalid == 1 &&
                          invalid.metrics_snapshot().consume_failed_invalid == 1,
                      "invalid batch metrics increment once");

    failures += check(broker.try_publish(42).is_ok(), "single publish still works");
    int value = 0;
    failures +=
        check(broker.try_consume(value).is_ok() && value == 42, "single consume still works");

    aether::BatchBroker<std::unique_ptr<int>, 4> move_broker;
    failures += check(move_broker.try_publish(std::make_unique<int>(9)).is_ok(),
                      "move-only single publish works");
    std::unique_ptr<int> moved;
    failures += check(move_broker.try_consume(moved).is_ok() && moved && *moved == 9,
                      "move-only single consume works");

    aether::BatchBroker<std::uint64_t, 1024> ordered;
    std::vector<std::uint64_t> in(1000);
    for (std::uint64_t i = 0; i < in.size(); ++i) {
        in[i] = i;
    }
    failures += check(ordered.try_publish_batch(in) == in.size(), "1000-item publish succeeds");
    std::vector<std::uint64_t> out(1000);
    failures += check(ordered.try_consume_batch(out) == out.size(), "1000-item consume succeeds");
    failures += check(out == in, "batch broker preserves FIFO for 1000 items");

    if (failures != 0) {
        return 1;
    }
    std::cout << "batch broker test passed\n";
    return 0;
}
