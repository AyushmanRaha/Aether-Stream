#pragma once

#include <aether/core/expected.hpp>
#include <aether/core/status.hpp>
#include <cstddef>
#include <filesystem>
#include <span>

namespace aether::io {

struct MmapFileOptions {
    bool flush_on_destroy{true};
};

class MmapFile {
public:
    MmapFile() noexcept = default;
    ~MmapFile() noexcept;

    MmapFile(const MmapFile&) = delete;
    MmapFile& operator=(const MmapFile&) = delete;

    MmapFile(MmapFile&& other) noexcept;
    MmapFile& operator=(MmapFile&& other) noexcept;

    [[nodiscard]] static Expected<MmapFile> create(const std::filesystem::path& path,
                                                   std::size_t size, MmapFileOptions options = {});

    [[nodiscard]] static Expected<MmapFile> open_existing(const std::filesystem::path& path,
                                                          MmapFileOptions options = {});

    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::byte* data() noexcept;
    [[nodiscard]] const std::byte* data() const noexcept;
    [[nodiscard]] std::span<std::byte> bytes() noexcept;
    [[nodiscard]] std::span<const std::byte> bytes() const noexcept;
    [[nodiscard]] const std::filesystem::path& path() const noexcept;

    [[nodiscard]] Status flush() noexcept;
    [[nodiscard]] Status resize(std::size_t new_size);
    [[nodiscard]] Status close() noexcept;

private:
    MmapFile(int fd, std::byte* data, std::size_t size, std::filesystem::path path,
             MmapFileOptions options) noexcept;

    void reset() noexcept;

    int fd_{-1};
    std::byte* data_{nullptr};
    std::size_t size_{0};
    std::filesystem::path path_{};
    MmapFileOptions options_{};
};

} // namespace aether::io
