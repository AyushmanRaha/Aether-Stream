#include <aether/broker.hpp>
#include <aether/wal/record.hpp>

namespace aether {

std::string_view broker_durability_mode_name(BrokerDurabilityMode mode) noexcept {
    switch (mode) {
    case BrokerDurabilityMode::in_memory:
        return "in_memory";
    case BrokerDurabilityMode::wal_before_queue:
        return "wal_before_queue";
    }
    return "unknown";
}

namespace detail {

Status validate_queue_config_for_capacity(const QueueConfig& config,
                                          std::size_t static_capacity) noexcept {
    const Status status = config.validate();
    if (!status.is_ok()) {
        return status;
    }
    if (config.capacity != static_capacity) {
        return Status{StatusCode::invalid_argument,
                      "queue config capacity does not match broker template capacity"};
    }
    return Status::ok();
}

Status validate_wal_config_for_payload(const WalConfig& config, std::size_t payload_size) noexcept {
    const Status status = config.validate();
    if (!status.is_ok()) {
        return status;
    }
    if (payload_size > max_payload_size) {
        return Status{StatusCode::invalid_argument, "broker payload too large for WAL record"};
    }
    if (config.file_size_bytes < wal::wal_record_header_size + payload_size) {
        return Status{StatusCode::invalid_argument,
                      "wal file size must fit at least one broker record"};
    }
    return Status::ok();
}

} // namespace detail
} // namespace aether
