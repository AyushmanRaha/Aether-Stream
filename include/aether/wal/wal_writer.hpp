#pragma once

#include <aether/core/config.hpp>
#include <aether/core/expected.hpp>
#include <aether/core/status.hpp>
#include <aether/io/mmap_file.hpp>
#include <aether/message.hpp>
#include <aether/wal/record.hpp>
#include <cstdint>
#include <filesystem>

namespace aether::wal {

class WalWriter {
public:
    WalWriter() noexcept = default;

    WalWriter(const WalWriter&) = delete;
    WalWriter& operator=(const WalWriter&) = delete;

    WalWriter(WalWriter&&) noexcept = default;
    WalWriter& operator=(WalWriter&&) noexcept = default;

    [[nodiscard]] static Expected<WalWriter> create(const WalConfig& config);
    [[nodiscard]] static Expected<WalWriter> create(const std::filesystem::path& path,
                                                    byte_count_t file_size_bytes,
                                                    bool flush_on_append = false);

    [[nodiscard]] Status append(MessageView message);
    [[nodiscard]] Status flush() noexcept;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] byte_count_t current_offset() const noexcept;
    [[nodiscard]] byte_count_t remaining_space() const noexcept;
    [[nodiscard]] sequence_t next_sequence() const noexcept;
    [[nodiscard]] std::uint64_t records_written() const noexcept;

private:
    WalWriter(io::MmapFile file, bool flush_on_append) noexcept;

    io::MmapFile file_{};
    byte_count_t current_offset_{};
    sequence_t next_sequence_{};
    std::uint64_t records_written_{};
    bool flush_on_append_{};
};

} // namespace aether::wal
