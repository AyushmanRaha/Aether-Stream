#include <aether/broker.hpp>
#include <cstdint>
#include <iostream>
#include <memory>

namespace {

int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }
    return 0;
}

struct EmplacedEvent {
    int id{};
    double value{};

    EmplacedEvent() = default;
    EmplacedEvent(int event_id, double event_value) : id(event_id), value(event_value) {}
};

} // namespace

int main() {
    int failures = 0;

    aether::Broker<int, 4> broker;
    int out = 0;
    failures += check(broker.empty(), "new broker should be empty");
    failures +=
        check(broker.metrics_snapshot().published == 0 && broker.metrics_snapshot().consumed == 0,
              "new broker metrics start at zero");
    failures += check(broker.try_consume(out).code() == aether::StatusCode::empty,
                      "empty consume should return empty status");
    failures += check(broker.metrics_snapshot().consume_failed_empty == 1,
                      "empty consume increments metric");
    failures += check(broker.try_publish(42).is_ok(), "single publish should succeed");
    failures +=
        check(broker.metrics_snapshot().published == 1, "successful publish increments metric");
    failures += check(broker.try_consume(out).is_ok(), "single consume should succeed");
    failures +=
        check(broker.metrics_snapshot().consumed == 1, "successful consume increments metric");
    failures += check(out == 42, "consumed value should match published value");
    broker.reset_metrics();
    failures += check(broker.metrics_snapshot().published == 0 &&
                          broker.metrics_snapshot().consume_failed_empty == 0,
                      "reset metrics clears broker counters");

    aether::Broker<int, 2> small_broker;
    failures += check(small_broker.try_publish(1).is_ok(), "small broker first publish succeeds");
    failures += check(small_broker.try_publish(2).is_ok(), "small broker second publish succeeds");
    failures += check(small_broker.try_publish(3).code() == aether::StatusCode::full,
                      "small broker third publish returns full");
    failures += check(small_broker.metrics_snapshot().publish_failed_full == 1,
                      "full publish increments metric");

    aether::Broker<std::uint64_t, 1024> ordered_broker;
    for (std::uint64_t value = 0; value < 1000; ++value) {
        failures += check(ordered_broker.try_publish(value).is_ok(), "ordered publish succeeds");
    }
    for (std::uint64_t expected = 0; expected < 1000; ++expected) {
        std::uint64_t actual = 9999;
        failures += check(ordered_broker.try_consume(actual).is_ok(), "ordered consume succeeds");
        failures += check(actual == expected, "broker preserves FIFO order");
    }

    aether::Broker<EmplacedEvent, 8> emplace_broker;
    failures += check(emplace_broker.try_emplace(7, 12.5).is_ok(), "try_emplace succeeds");
    EmplacedEvent event{};
    failures += check(emplace_broker.try_consume(event).is_ok(), "emplaced event consumes");
    failures += check(event.id == 7 && event.value == 12.5, "emplaced event fields match");

    aether::Broker<std::unique_ptr<int>, 4> move_broker;
    failures += check(move_broker.try_publish(std::make_unique<int>(42)).is_ok(),
                      "move-only publish succeeds");
    std::unique_ptr<int> moved_out;
    failures += check(move_broker.try_consume(moved_out).is_ok(), "move-only consume succeeds");
    failures += check(moved_out != nullptr && *moved_out == 42, "move-only value matches");

    aether::QueueConfig config{};
    config.capacity = 8;
    aether::Broker<int, 8> valid_broker(config);
    aether::Broker<int, 4> invalid_broker(config);
    failures += check(valid_broker.valid(), "matching runtime capacity is valid");
    failures += check(!invalid_broker.valid(), "mismatched runtime capacity is invalid");
    failures += check(invalid_broker.config_status().code() == aether::StatusCode::invalid_argument,
                      "mismatched runtime capacity returns invalid_argument");
    failures += check(invalid_broker.try_publish(1).code() == aether::StatusCode::invalid_argument,
                      "invalid broker publish returns invalid_argument");
    failures +=
        check(invalid_broker.try_consume(out).code() == aether::StatusCode::invalid_argument,
              "invalid broker consume returns invalid_argument");
    failures += check(invalid_broker.metrics_snapshot().publish_failed_invalid == 1 &&
                          invalid_broker.metrics_snapshot().consume_failed_invalid == 1,
                      "invalid broker metrics increment");
    failures += check(
        aether::broker_durability_mode_name(aether::BrokerDurabilityMode::in_memory) == "in_memory",
        "in-memory durability mode name matches");

    if (failures != 0) {
        return 1;
    }

    std::cout << "broker test passed\n";
    return 0;
}
