// Demonstrates basic Broker publish/consume behavior over the SPSC queue.
#include <aether/broker.hpp>
#include <cstdint>
#include <iostream>

struct OrderEvent {
    std::uint64_t id{};
    double price{};
    std::uint32_t quantity{};
};

int main() {
    aether::Broker<OrderEvent, 64> broker;

    const auto publish_status = broker.try_publish(OrderEvent{1, 101.25, 10});
    if (!publish_status) {
        std::cerr << "publish failed: " << publish_status.message() << '\n';
        return 1;
    }

    OrderEvent out{};
    const auto consume_status = broker.try_consume(out);
    if (!consume_status) {
        std::cerr << "consume failed: " << consume_status.message() << '\n';
        return 1;
    }

    std::cout << "consumed order id=" << out.id << " price=" << out.price
              << " quantity=" << out.quantity << '\n';
    return 0;
}
