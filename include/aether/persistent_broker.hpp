#pragma once

#include <aether/broker.hpp>
#include <aether/message.hpp>
#include <aether/wal/record.hpp>
#include <aether/wal/wal_reader.hpp>
#include <aether/wal/wal_writer.hpp>
#include <cstring>
#include <filesystem>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>

namespace aether {

template <typename T, std::size_t Capacity> class PersistentBroker {
public:
    using value_type = T;

    static_assert(std::is_trivially_copyable_v<T>,
                  "PersistentBroker requires trivially copyable payload types");
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
    static_assert(Capacity >= 2, "Capacity must be at least 2");

    PersistentBroker() noexcept = default;
    explicit PersistentBroker(const WalConfig& wal_config) {
        static_cast<void>(open(wal_config));
    }
    explicit PersistentBroker(const BrokerConfig& config) {
        static_cast<void>(open(config));
    }

    PersistentBroker(const PersistentBroker&) = delete;
    PersistentBroker& operator=(const PersistentBroker&) = delete;
    PersistentBroker(PersistentBroker&&) = delete;
    PersistentBroker& operator=(PersistentBroker&&) = delete;

    [[nodiscard]] Status open(const WalConfig& wal_config) {
        BrokerConfig config{};
        config.queue.capacity = Capacity;
        config.wal = wal_config;
        config.enable_wal = true;
        return open(config);
    }

    [[nodiscard]] Status open(const BrokerConfig& config) {
        if (writer_.valid()) {
            open_status_ =
                Status{StatusCode::invalid_argument, "persistent broker is already open"};
            return open_status_;
        }

        const Status queue_status =
            detail::validate_queue_config_for_capacity(config.queue, Capacity);
        if (!queue_status.is_ok()) {
            open_status_ = queue_status;
            return open_status_;
        }

        if (!config.enable_wal) {
            open_status_ = Status{StatusCode::invalid_argument,
                                  "persistent broker requires WAL to be enabled"};
            return open_status_;
        }

        const Status wal_status = detail::validate_wal_config_for_payload(config.wal, sizeof(T));
        if (!wal_status.is_ok()) {
            open_status_ = wal_status;
            return open_status_;
        }

        auto writer_result = wal::WalWriter::create(config.wal);
        if (!writer_result.has_value()) {
            open_status_ = writer_result.status();
            return open_status_;
        }

        writer_ = std::move(writer_result).value();
        open_status_ = Status::ok();
        return open_status_;
    }

    [[nodiscard]] bool valid() const noexcept {
        return open_status_.is_ok() && writer_.valid();
    }
    [[nodiscard]] Status open_status() const noexcept {
        return open_status_;
    }

    [[nodiscard]] Status try_publish(const T& value, message_flags_t flags = 0) {
        if (!valid()) {
            return open_status_;
        }
        if (broker_.full()) {
            return Status{StatusCode::full, "broker queue is full"};
        }

        const Status wal_status = append_to_wal(value, flags);
        if (!wal_status.is_ok()) {
            return wal_status;
        }

        const Status publish_status = broker_.try_publish(value);
        if (!publish_status.is_ok()) {
            return Status{StatusCode::unknown, "queue publish failed after WAL append"};
        }
        return Status::ok();
    }

    [[nodiscard]] Status try_publish(T&& value, message_flags_t flags = 0)
        requires(std::is_move_constructible_v<T>)
    {
        if (!valid()) {
            return open_status_;
        }
        if (broker_.full()) {
            return Status{StatusCode::full, "broker queue is full"};
        }

        const Status wal_status = append_to_wal(value, flags);
        if (!wal_status.is_ok()) {
            return wal_status;
        }

        const Status publish_status = broker_.try_publish(std::move(value));
        if (!publish_status.is_ok()) {
            return Status{StatusCode::unknown, "queue publish failed after WAL append"};
        }
        return Status::ok();
    }

    [[nodiscard]] Status try_consume(T& out) {
        if (!valid()) {
            return open_status_;
        }
        return broker_.try_consume(out);
    }

    [[nodiscard]] Status flush() noexcept {
        if (!valid()) {
            return open_status_;
        }
        return writer_.flush();
    }

    [[nodiscard]] bool empty() const noexcept {
        return broker_.empty();
    }
    [[nodiscard]] bool full() const noexcept {
        return broker_.full();
    }
    [[nodiscard]] std::size_t size_approx() const noexcept {
        return broker_.size_approx();
    }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept {
        return Capacity;
    }
    [[nodiscard]] byte_count_t wal_current_offset() const noexcept {
        return writer_.current_offset();
    }
    [[nodiscard]] byte_count_t wal_remaining_space() const noexcept {
        return writer_.remaining_space();
    }
    [[nodiscard]] sequence_t wal_next_sequence() const noexcept {
        return writer_.next_sequence();
    }
    [[nodiscard]] std::uint64_t wal_records_written() const noexcept {
        return writer_.records_written();
    }
    [[nodiscard]] static constexpr BrokerDurabilityMode durability_mode() noexcept {
        return BrokerDurabilityMode::wal_before_queue;
    }

    template <typename Visitor>
    [[nodiscard]] static Status replay(const std::filesystem::path& path, Visitor&& visitor) {
        static_assert(
            std::is_invocable_r_v<Status, Visitor&, const T&, const wal::WalRecordHeader&>,
            "PersistentBroker::replay visitor must return Status and accept "
            "(const T&, const wal::WalRecordHeader&)");

        auto reader_result = wal::WalReader::open(path);
        if (!reader_result.has_value()) {
            return reader_result.status();
        }

        auto reader = std::move(reader_result).value();
        auto& visitor_ref = visitor;
        return reader.replay([&visitor_ref](const wal::WalRecordView& record) {
            if (record.payload.size() != sizeof(T)) {
                return Status{StatusCode::corrupted_record,
                              "WAL payload size does not match persistent broker value type"};
            }

            T value{};
            std::memcpy(std::addressof(value), record.payload.data(), sizeof(T));
            return visitor_ref(value, record.header);
        });
    }

private:
    [[nodiscard]] static std::span<const std::byte> object_payload(const T& value) noexcept {
        return {reinterpret_cast<const std::byte*>(std::addressof(value)), sizeof(T)};
    }

    [[nodiscard]] Status append_to_wal(const T& value, message_flags_t flags) {
        MessageView message{MessageHeader{0, 0, static_cast<payload_size_t>(sizeof(T)), flags},
                            object_payload(value)};
        return writer_.append(message);
    }

    Broker<T, Capacity> broker_{};
    wal::WalWriter writer_{};
    Status open_status_{StatusCode::invalid_argument, "persistent broker is not open"};
};

} // namespace aether
