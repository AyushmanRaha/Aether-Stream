#include <aether/utils/clock.hpp>
#include <aether/wal/checksum.hpp>
#include <aether/wal/wal_writer.hpp>
#include <cstring>
#include <limits>
#include <utility>

namespace aether::wal {

namespace {

[[nodiscard]] bool fits_size_t(byte_count_t value) noexcept {
    return value <= static_cast<byte_count_t>(std::numeric_limits<std::size_t>::max());
}

} // namespace

WalWriter::WalWriter(io::MmapFile file, bool flush_on_append) noexcept
    : file_(std::move(file)), flush_on_append_(flush_on_append) {}

Expected<WalWriter> WalWriter::create(const WalConfig& config) {
    const Status status = config.validate();
    if (!status.is_ok()) {
        return status;
    }
    return create(config.path, config.file_size_bytes, config.flush_on_commit);
}

Expected<WalWriter> WalWriter::create(const std::filesystem::path& path,
                                      byte_count_t file_size_bytes, bool flush_on_append) {
    if (path.empty()) {
        return Status{StatusCode::invalid_argument, "wal path must not be empty"};
    }
    if (file_size_bytes < wal_record_header_size) {
        return Status{StatusCode::invalid_argument, "wal file cannot fit one header"};
    }
    if (!fits_size_t(file_size_bytes)) {
        return Status{StatusCode::invalid_argument, "wal file size too large for this platform"};
    }

    auto file = io::MmapFile::create(path, static_cast<std::size_t>(file_size_bytes));
    if (!file.has_value()) {
        return file.status();
    }

    return WalWriter{std::move(file).value(), flush_on_append};
}

Status WalWriter::append(MessageView message) {
    if (!valid()) {
        return Status{StatusCode::invalid_argument, "wal writer is not open"};
    }

    const Status message_status = validate_message_view(message);
    if (!message_status.is_ok()) {
        return message_status;
    }
    if (!fits_size_t(message.payload.size())) {
        return Status{StatusCode::invalid_argument, "payload too large"};
    }

    const auto payload_size = static_cast<byte_count_t>(message.payload.size());
    if (payload_size > max_payload_size) {
        return Status{StatusCode::invalid_argument, "payload too large"};
    }
    if (payload_size > std::numeric_limits<byte_count_t>::max() - wal_record_header_size) {
        return Status{StatusCode::invalid_argument, "wal record size overflow"};
    }

    const byte_count_t total_size = wal_record_header_size + payload_size;
    if (total_size > remaining_space()) {
        return Status{StatusCode::out_of_space, "wal file does not have enough remaining space"};
    }
    if (!fits_size_t(current_offset_) || !fits_size_t(total_size)) {
        return Status{StatusCode::invalid_argument, "wal offset too large for this platform"};
    }

    WalRecordHeader header{};
    header.payload_size = static_cast<std::uint32_t>(message.payload.size());
    header.sequence = next_sequence_;
    header.timestamp_ns =
        message.header.timestamp_ns != 0 ? message.header.timestamp_ns : utils::monotonic_time_ns();
    header.flags = message.header.flags;
    header.checksum = compute_record_checksum(header, message.payload);

    const auto serialized = serialize_header(header);
    auto bytes = file_.bytes();
    auto* destination = bytes.data() + static_cast<std::size_t>(current_offset_);
    std::memcpy(destination, serialized.data(), serialized.size());
    if (!message.payload.empty()) {
        std::memcpy(destination + wal_record_header_size, message.payload.data(),
                    message.payload.size());
    }

    if (flush_on_append_) {
        const Status flush_status = flush();
        if (!flush_status.is_ok()) {
            return flush_status;
        }
    }

    current_offset_ += total_size;
    ++next_sequence_;
    ++records_written_;
    return Status::ok();
}

Status WalWriter::flush() noexcept {
    if (!valid()) {
        return Status{StatusCode::invalid_argument, "wal writer is not open"};
    }
    return file_.flush();
}

bool WalWriter::valid() const noexcept {
    return file_.valid();
}

byte_count_t WalWriter::current_offset() const noexcept {
    return current_offset_;
}

byte_count_t WalWriter::remaining_space() const noexcept {
    if (!valid()) {
        return 0;
    }
    const auto size = static_cast<byte_count_t>(file_.size());
    return current_offset_ <= size ? size - current_offset_ : 0;
}

sequence_t WalWriter::next_sequence() const noexcept {
    return next_sequence_;
}

std::uint64_t WalWriter::records_written() const noexcept {
    return records_written_;
}

} // namespace aether::wal
