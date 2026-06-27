#include <aether/broker.hpp>
#include <aether/cli/args.hpp>
#include <aether/persistent_broker.hpp>
#include <iostream>
#include <type_traits>

namespace {
struct OrderEvent {
    std::uint64_t id{};
    double price{};
    std::uint32_t quantity{};
};
static_assert(std::is_trivially_copyable_v<OrderEvent>);
OrderEvent make_event(std::uint64_t i) {
    return {i + 1U, 100.0 + static_cast<double>(i % 100U) * 0.25,
            1U + static_cast<std::uint32_t>(i % 1000U)};
}
void print_event(const OrderEvent& event) {
    std::cout << "id=" << event.id << " price=" << event.price << " quantity=" << event.quantity
              << '\n';
}
void print_error(aether::Status status) {
    std::cerr << "error: " << status.message() << '\n';
}

template <std::size_t Capacity> int run_local(const aether::cli::SubOptions& options) {
    aether::Broker<OrderEvent, Capacity> broker{};
    for (std::uint64_t i = 0; i < options.messages; ++i) {
        if (const auto status = broker.try_publish(make_event(i)); !status) {
            print_error(status);
            return 1;
        }
    }
    std::uint64_t consumed{};
    while (consumed < options.messages) {
        OrderEvent event{};
        if (const auto status = broker.try_consume(event); !status) {
            print_error(status);
            return 1;
        }
        if (consumed < options.limit) {
            print_event(event);
        }
        ++consumed;
    }
    const auto suppressed = consumed > options.limit ? consumed - options.limit : 0U;
    std::cout << "summary: produced=" << options.messages << " consumed=" << consumed
              << " suppressed=" << suppressed << '\n';
    return 0;
}

int dispatch_local(const aether::cli::SubOptions& options) {
    switch (options.capacity) {
    case 64:
        return run_local<64>(options);
    case 256:
        return run_local<256>(options);
    case 1024:
        return run_local<1024>(options);
    case 4096:
        return run_local<4096>(options);
    case 65536:
        return run_local<65536>(options);
    default:
        return 2;
    }
}
} // namespace

int main(int argc, char** argv) {
    const auto parsed = aether::cli::parse_sub_args({argv, static_cast<std::size_t>(argc)});
    if (!parsed) {
        print_error(parsed.status());
        return 2;
    }
    const auto options = parsed.value();
    if (options.help) {
        std::cout << aether::cli::sub_help();
        return 0;
    }
    std::cout
        << "Phase 9 local demo subscriber: no networking and no remote/live broker subscription.\n";
    if (options.wal_path.empty()) {
        return dispatch_local(options);
    }
    std::uint64_t seen{};
    const auto status = aether::PersistentBroker<OrderEvent, 65536>::replay(
        options.wal_path, [&](const OrderEvent& event, const aether::wal::WalRecordHeader&) {
            if (seen < options.limit) {
                print_event(event);
            }
            ++seen;
            return aether::Status::ok();
        });
    if (!status) {
        print_error(status);
        return 1;
    }
    const auto suppressed = seen > options.limit ? seen - options.limit : 0U;
    std::cout << "summary: replayed=" << seen
              << " printed=" << (seen < options.limit ? seen : options.limit)
              << " suppressed=" << suppressed << '\n';
}
