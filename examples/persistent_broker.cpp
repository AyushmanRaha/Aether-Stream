// Demonstrates PersistentBroker WAL-before-queue publish and replay behavior.
#include <aether/persistent_broker.hpp>
#include <cstdint>
#include <filesystem>
#include <iostream>

struct OrderEvent {
    std::uint64_t id{};
    double price{};
    std::uint32_t quantity{};
};

int main() {
    const auto path =
        std::filesystem::temp_directory_path() / "aether-persistent-broker-example.wal";
    std::filesystem::remove(path);

    {
        aether::WalConfig wal_config{};
        wal_config.path = path;
        wal_config.file_size_bytes = 4096;
        wal_config.flush_on_commit = false;

        aether::PersistentBroker<OrderEvent, 64> broker(wal_config);
        if (!broker.valid()) {
            std::cerr << "open failed: " << broker.open_status().message() << '\n';
            return 1;
        }

        for (const auto order : {OrderEvent{1, 101.25, 10}, OrderEvent{2, 102.5, 20}}) {
            const auto publish_status = broker.try_publish(order);
            if (!publish_status) {
                std::cerr << "publish failed: " << publish_status.message() << '\n';
                return 1;
            }
            std::cout << "published order id=" << order.id << '\n';
        }

        for (int i = 0; i < 2; ++i) {
            OrderEvent out{};
            const auto consume_status = broker.try_consume(out);
            if (!consume_status) {
                std::cerr << "consume failed: " << consume_status.message() << '\n';
                return 1;
            }
            std::cout << "consumed order id=" << out.id << " price=" << out.price
                      << " quantity=" << out.quantity << '\n';
        }

        const auto flush_status = broker.flush();
        if (!flush_status) {
            std::cerr << "flush failed: " << flush_status.message() << '\n';
            return 1;
        }
    }

    std::cout << "replaying WAL from " << path << '\n';
    const auto replay_status = aether::PersistentBroker<OrderEvent, 64>::replay(
        path, [](const OrderEvent& order, const aether::wal::WalRecordHeader& header) {
            std::cout << "replayed sequence=" << header.sequence << " order id=" << order.id
                      << " price=" << order.price << " quantity=" << order.quantity << '\n';
            return aether::Status::ok();
        });

    if (!replay_status) {
        std::cerr << "replay failed: " << replay_status.message() << '\n';
        std::filesystem::remove(path);
        return 1;
    }

    std::filesystem::remove(path);
    return 0;
}
