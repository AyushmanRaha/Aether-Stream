#pragma once

#include <aether/core/status.hpp>
#include <aether/core/types.hpp>
#include <filesystem>

namespace aether {

struct QueueConfig {
    std::size_t capacity{default_queue_capacity};
    std::uint32_t spin_pause_iterations{64};
    bool yield_after_spin{true};

    [[nodiscard]] Status validate() const noexcept {
        if (capacity == 0) {
            return Status{StatusCode::invalid_argument, "queue capacity must be nonzero"};
        }
        if (!is_power_of_two(capacity)) {
            return Status{StatusCode::invalid_argument, "queue capacity must be a power of two"};
        }

        return Status::ok();
    }
};

struct WalConfig {
    std::filesystem::path path{"aether.wal"};
    byte_count_t file_size_bytes{default_wal_size_bytes};
    bool flush_on_commit{false};

    [[nodiscard]] Status validate() const {
        if (path.empty()) {
            return Status{StatusCode::invalid_argument, "wal path must not be empty"};
        }
        if (file_size_bytes == 0) {
            return Status{StatusCode::invalid_argument, "wal file size must be nonzero"};
        }

        return Status::ok();
    }
};

struct BrokerConfig {
    QueueConfig queue{};
    WalConfig wal{};
    bool enable_wal{false};

    [[nodiscard]] Status validate() const {
        const Status queue_status = queue.validate();
        if (!queue_status.is_ok()) {
            return queue_status;
        }

        if (enable_wal) {
            return wal.validate();
        }

        return Status::ok();
    }
};

} // namespace aether
