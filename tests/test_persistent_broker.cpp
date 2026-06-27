#include <aether/persistent_broker.hpp>
#include <aether/wal/wal_reader.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <span>
#include <string>
#include <type_traits>
#include <unistd.h>

namespace {

struct OrderEvent {
    std::uint64_t id{};
    double price{};
    std::uint32_t quantity{};
};
static_assert(std::is_trivially_copyable_v<OrderEvent>);

int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }
    return 0;
}

std::filesystem::path unique_path(const char* suffix) {
    return std::filesystem::temp_directory_path() /
           (std::string{"aether-persistent-broker-"} + std::to_string(::getpid()) + suffix);
}

OrderEvent payload_to_order(std::span<const std::byte> payload) {
    OrderEvent event{};
    std::memcpy(&event, payload.data(), sizeof(event));
    return event;
}

bool same_event(const OrderEvent& lhs, const OrderEvent& rhs) {
    return lhs.id == rhs.id && lhs.price == rhs.price && lhs.quantity == rhs.quantity;
}

} // namespace

int main() {
    int failures = 0;

    {
        aether::PersistentBroker<OrderEvent, 8> broker;
        failures += check(!broker.valid(), "default persistent broker is invalid");
        failures += check(broker.try_publish(OrderEvent{1, 10.0, 3}).code() ==
                              aether::StatusCode::invalid_argument,
                          "unopened publish returns invalid_argument");
    }

    {
        aether::WalConfig config{};
        config.path.clear();
        aether::PersistentBroker<OrderEvent, 8> broker(config);
        failures += check(!broker.valid(), "empty WAL path is invalid");
        failures += check(broker.open_status().code() == aether::StatusCode::invalid_argument,
                          "empty WAL path returns invalid_argument");
    }

    {
        const auto path = unique_path("-too-small.wal");
        std::filesystem::remove(path);
        aether::WalConfig config{};
        config.path = path;
        config.file_size_bytes = aether::wal::wal_record_header_size + sizeof(OrderEvent) - 1U;
        aether::PersistentBroker<OrderEvent, 8> broker(config);
        failures += check(!broker.valid(), "too-small WAL is invalid");
        failures += check(broker.open_status().code() == aether::StatusCode::invalid_argument,
                          "too-small WAL returns invalid_argument");
        std::filesystem::remove(path);
    }

    const auto path = unique_path("-main.wal");
    std::filesystem::remove(path);
    const std::array<OrderEvent, 3> expected{{{1, 101.25, 10}, {2, 102.5, 20}, {3, 103.75, 30}}};
    {
        aether::WalConfig wal_config{};
        wal_config.path = path;
        wal_config.file_size_bytes = 4096;
        wal_config.flush_on_commit = false;
        aether::PersistentBroker<OrderEvent, 8> broker(wal_config);
        failures += check(broker.valid(), "persistent broker opens");

        failures += check(broker.try_publish(expected[0], 0xA).is_ok(),
                          "first persistent publish succeeds");
        failures += check(broker.wal_records_written() == 1, "records written increments to one");
        failures += check(broker.wal_next_sequence() == 1, "next sequence increments to one");
        failures += check(broker.try_publish(expected[1], 0xB).is_ok(),
                          "second persistent publish succeeds");
        failures += check(broker.try_publish(expected[2], 0xC).is_ok(),
                          "third persistent publish succeeds");
        failures += check(broker.wal_records_written() == expected.size(),
                          "records written matches publishes");
        failures +=
            check(broker.wal_next_sequence() == expected.size(), "next sequence matches publishes");

        for (const auto& wanted : expected) {
            OrderEvent out{};
            failures += check(broker.try_consume(out).is_ok(), "persistent consume succeeds");
            failures += check(same_event(out, wanted), "persistent consume preserves order");
        }
        OrderEvent out{};
        failures += check(broker.try_consume(out).code() == aether::StatusCode::empty,
                          "persistent empty consume returns empty");
        failures += check(broker.flush().is_ok(), "persistent broker flush succeeds");
    }

    {
        auto reader_result = aether::wal::WalReader::open(path);
        failures += check(reader_result.has_value(), "WAL reader opens broker WAL");
        if (reader_result.has_value()) {
            auto reader = std::move(reader_result).value();
            for (std::size_t index = 0; index < expected.size(); ++index) {
                auto record = reader.next();
                failures += check(record.has_value(), "WAL record reads");
                if (record.has_value()) {
                    failures +=
                        check(record.value().header.sequence == index, "WAL sequence matches");
                    failures += check(record.value().payload.size() == sizeof(OrderEvent),
                                      "WAL payload size matches event size");
                    const OrderEvent decoded = payload_to_order(record.value().payload);
                    failures += check(same_event(decoded, expected[index]), "WAL payload decodes");
                    failures += check(record.value().header.flags ==
                                          static_cast<std::uint32_t>(0xA + index),
                                      "WAL flags match");
                }
            }
            failures += check(reader.next().status().code() == aether::StatusCode::empty,
                              "WAL EOF returns empty");
        }
    }

    {
        std::uint64_t visited = 0;
        const aether::Status replay_status = aether::PersistentBroker<OrderEvent, 8>::replay(
            path, [&](const OrderEvent& event, const aether::wal::WalRecordHeader& header) {
                const auto index = static_cast<std::size_t>(header.sequence);
                failures += check(index < expected.size(), "replay sequence is in range");
                if (index < expected.size()) {
                    failures +=
                        check(same_event(event, expected[index]), "typed replay event matches");
                }
                ++visited;
                return aether::Status::ok();
            });
        failures += check(replay_status.is_ok(), "typed replay succeeds");
        failures += check(visited == expected.size(), "typed replay visits all records");

        visited = 0;
        const aether::Status stop_status = aether::PersistentBroker<OrderEvent, 8>::replay(
            path, [&](const OrderEvent&, const aether::wal::WalRecordHeader&) {
                ++visited;
                return aether::Status{aether::StatusCode::invalid_argument, "stop replay"};
            });
        failures += check(stop_status.code() == aether::StatusCode::invalid_argument,
                          "typed replay propagates visitor status");
        failures += check(visited == 1, "typed replay stops after visitor error");
    }

    const auto full_path = unique_path("-full.wal");
    std::filesystem::remove(full_path);
    {
        aether::WalConfig wal_config{};
        wal_config.path = full_path;
        wal_config.file_size_bytes = 4096;
        aether::PersistentBroker<OrderEvent, 2> broker(wal_config);
        failures += check(broker.valid(), "full-queue broker opens");
        failures +=
            check(broker.try_publish(expected[0]).is_ok(), "full test first publish succeeds");
        failures +=
            check(broker.try_publish(expected[1]).is_ok(), "full test second publish succeeds");
        failures += check(broker.try_publish(expected[2]).code() == aether::StatusCode::full,
                          "full queue returns full");
        failures += check(broker.flush().is_ok(), "full-queue broker flush succeeds");
    }
    {
        std::uint64_t records = 0;
        const aether::Status replay_status = aether::PersistentBroker<OrderEvent, 2>::replay(
            full_path, [&](const OrderEvent&, const aether::wal::WalRecordHeader&) {
                ++records;
                return aether::Status::ok();
            });
        failures += check(replay_status.is_ok(), "full-queue replay succeeds");
        failures += check(records == 2, "full queue does not append extra WAL record");
    }

    std::filesystem::remove(path);
    std::filesystem::remove(full_path);

    if (failures != 0) {
        return 1;
    }

    std::cout << "persistent broker test passed\n";
    return 0;
}
