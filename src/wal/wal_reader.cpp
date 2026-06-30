#include <aether/wal/checksum.hpp>
#include <aether/wal/wal_reader.hpp>
#include <limits>
#include <utility>

namespace aether::wal {

namespace {

[[nodiscard]] bool fits_size_t(byte_count_t value) noexcept {
    return value <= static_cast<byte_count_t>(std::numeric_limits<std::size_t>::max());
}

} // namespace

WalReader::WalReader(io::MmapFile file) noexcept : file_(std::move(file)) {}

Expected<WalReader> WalReader::open(const std::filesystem::path& path) {
    auto file = io::MmapFile::open_existing(path, {.flush_on_destroy = false});
    if (!file.has_value()) {
        return file.status();
    }
    return WalReader{std::move(file).value()};
}

Expected<WalRecordView> WalReader::next() noexcept {
    if (!valid()) {
        return Status{StatusCode::invalid_argument, "wal reader is not open"};
    }

    const auto file_size = static_cast<byte_count_t>(file_.size());
    if (current_offset_ >= file_size) {
        return Status{StatusCode::empty};
    }

    const byte_count_t remaining = file_size - current_offset_;
    const auto all_bytes = file_.bytes();
    if (!fits_size_t(current_offset_) || !fits_size_t(remaining)) {
        return Status{StatusCode::invalid_argument, "wal offset too large for this platform"};
    }
    const auto offset = static_cast<std::size_t>(current_offset_);
    const auto remaining_size = static_cast<std::size_t>(remaining);
    const auto tail = all_bytes.subspan(offset, remaining_size);

    if (remaining < wal_record_header_size) {
        // A short zero tail is preallocated-file slack; a non-zero short tail is a partial record.
        if (is_zero_filled(tail)) {
            return Status{StatusCode::empty};
        }
        return Status{StatusCode::empty, "partial wal record"};
    }

    auto header_result = deserialize_header(tail.first(wal_record_header_size));
    if (!header_result.has_value()) {
        return header_result.status();
    }
    const WalRecordHeader header = header_result.value();

    if (header.magic == 0 && is_zero_filled(tail)) {
        return Status{StatusCode::empty};
    }

    const Status header_status = validate_header_shape(header);
    if (!header_status.is_ok()) {
        return header_status;
    }

    const byte_count_t total_size = record_total_size(header);
    if (total_size < header.header_size || total_size > remaining) {
        // Treat an incomplete final record as a clean stop so replay can ignore torn
        // tails.
        return Status{StatusCode::empty, "partial wal record"};
    }
    if (!fits_size_t(total_size)) {
        return Status{StatusCode::corrupted_record, "wal record too large for this platform"};
    }

    const auto payload = tail.subspan(wal_record_header_size, header.payload_size);
    if (!validate_record_checksum(header, payload)) {
        return Status{StatusCode::corrupted_record, "wal checksum mismatch"};
    }

    WalRecordView view{header, payload, current_offset_};
    current_offset_ += total_size;
    ++records_read_;
    return view;
}

Status WalReader::reset() noexcept {
    if (!valid()) {
        return Status{StatusCode::invalid_argument, "wal reader is not open"};
    }
    current_offset_ = 0;
    records_read_ = 0;
    return Status::ok();
}

bool WalReader::valid() const noexcept {
    return file_.valid();
}

byte_count_t WalReader::current_offset() const noexcept {
    return current_offset_;
}

std::uint64_t WalReader::records_read() const noexcept {
    return records_read_;
}

} // namespace aether::wal
