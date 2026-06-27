#pragma once

#include <aether/core/expected.hpp>
#include <aether/core/status.hpp>
#include <aether/io/mmap_file.hpp>
#include <aether/wal/record.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <utility>

namespace aether::wal {

class WalReader {
public:
    WalReader() noexcept = default;

    WalReader(const WalReader&) = delete;
    WalReader& operator=(const WalReader&) = delete;

    WalReader(WalReader&&) noexcept = default;
    WalReader& operator=(WalReader&&) noexcept = default;

    [[nodiscard]] static Expected<WalReader> open(const std::filesystem::path& path);

    [[nodiscard]] Expected<WalRecordView> next() noexcept;
    [[nodiscard]] Status reset() noexcept;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] byte_count_t current_offset() const noexcept;
    [[nodiscard]] std::uint64_t records_read() const noexcept;

    template <typename Visitor> [[nodiscard]] Status replay(Visitor&& visitor) {
        while (true) {
            Expected<WalRecordView> record = next();
            if (record.has_value()) {
                const Status status = std::forward<Visitor>(visitor)(record.value());
                if (!status.is_ok()) {
                    return status;
                }
                continue;
            }

            if (record.status().code() == StatusCode::empty) {
                return Status::ok();
            }
            return record.status();
        }
    }

private:
    explicit WalReader(io::MmapFile file) noexcept;

    io::MmapFile file_{};
    byte_count_t current_offset_{};
    std::uint64_t records_read_{};
};

} // namespace aether::wal
