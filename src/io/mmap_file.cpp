#include <aether/io/mmap_file.hpp>
#include <cstddef>
#include <filesystem>
#include <string_view>
#include <utility>

#if defined(_WIN32)
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace aether::io {

namespace {

constexpr std::string_view invalid_path_detail = "mmap file path must not be empty";
constexpr std::string_view invalid_size_detail = "mmap file size must be greater than zero";
constexpr std::string_view invalid_file_detail = "mmap file is not open";
constexpr std::string_view open_failed_detail = "failed to open mmap file";
constexpr std::string_view fstat_failed_detail = "failed to stat mmap file";
constexpr std::string_view truncate_failed_detail = "failed to resize mmap file";
constexpr std::string_view map_failed_detail = "failed to map mmap file";
constexpr std::string_view flush_failed_detail = "failed to flush mmap file";
constexpr std::string_view unmap_failed_detail = "failed to unmap mmap file";
constexpr std::string_view close_failed_detail = "failed to close mmap file";
#if defined(_WIN32)
constexpr std::string_view unsupported_detail = "mmap file is only implemented for POSIX platforms";
#endif

[[nodiscard]] Status invalid_path_status() noexcept {
    return Status{StatusCode::invalid_argument, invalid_path_detail};
}

[[nodiscard]] Status invalid_size_status() noexcept {
    return Status{StatusCode::invalid_argument, invalid_size_detail};
}

} // namespace

MmapFile::MmapFile(int fd, std::byte* data, std::size_t size, std::filesystem::path path,
                   MmapFileOptions options) noexcept
    : fd_(fd), data_(data), size_(size), path_(std::move(path)), options_(options) {}

MmapFile::~MmapFile() noexcept {
    static_cast<void>(close());
}

MmapFile::MmapFile(MmapFile&& other) noexcept
    : fd_(std::exchange(other.fd_, -1)), data_(std::exchange(other.data_, nullptr)),
      size_(std::exchange(other.size_, 0)), path_(std::move(other.path_)),
      options_(other.options_) {}

MmapFile& MmapFile::operator=(MmapFile&& other) noexcept {
    if (this != &other) {
        static_cast<void>(close());
        fd_ = std::exchange(other.fd_, -1);
        data_ = std::exchange(other.data_, nullptr);
        size_ = std::exchange(other.size_, 0);
        path_ = std::move(other.path_);
        options_ = other.options_;
    }

    return *this;
}

Expected<MmapFile> MmapFile::create(const std::filesystem::path& path, std::size_t size,
                                    MmapFileOptions options) {
    if (path.empty()) {
        return invalid_path_status();
    }
    if (size == 0) {
        return invalid_size_status();
    }

#if defined(_WIN32)
    static_cast<void>(path);
    static_cast<void>(options);
    return Status{StatusCode::io_error, unsupported_detail};
#else
    const int fd = ::open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        return Status{StatusCode::io_error, open_failed_detail};
    }

    if (::ftruncate(fd, static_cast<off_t>(size)) == -1) {
        static_cast<void>(::close(fd));
        return Status{StatusCode::io_error, truncate_failed_detail};
    }

    void* mapping = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapping == MAP_FAILED) {
        static_cast<void>(::close(fd));
        return Status{StatusCode::io_error, map_failed_detail};
    }

    return MmapFile{fd, static_cast<std::byte*>(mapping), size, path, options};
#endif
}

Expected<MmapFile> MmapFile::open_existing(const std::filesystem::path& path,
                                           MmapFileOptions options) {
    if (path.empty()) {
        return invalid_path_status();
    }

#if defined(_WIN32)
    static_cast<void>(path);
    static_cast<void>(options);
    return Status{StatusCode::io_error, unsupported_detail};
#else
    const int fd = ::open(path.c_str(), O_RDWR);
    if (fd == -1) {
        return Status{StatusCode::io_error, open_failed_detail};
    }

    struct stat file_stat{};
    if (::fstat(fd, &file_stat) == -1) {
        static_cast<void>(::close(fd));
        return Status{StatusCode::io_error, fstat_failed_detail};
    }

    if (file_stat.st_size <= 0) {
        static_cast<void>(::close(fd));
        return invalid_size_status();
    }

    const auto size = static_cast<std::size_t>(file_stat.st_size);
    void* mapping = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapping == MAP_FAILED) {
        static_cast<void>(::close(fd));
        return Status{StatusCode::io_error, map_failed_detail};
    }

    return MmapFile{fd, static_cast<std::byte*>(mapping), size, path, options};
#endif
}

bool MmapFile::valid() const noexcept {
    return fd_ != -1 && data_ != nullptr && size_ != 0;
}

std::size_t MmapFile::size() const noexcept {
    return size_;
}

std::byte* MmapFile::data() noexcept {
    return valid() ? data_ : nullptr;
}

const std::byte* MmapFile::data() const noexcept {
    return valid() ? data_ : nullptr;
}

std::span<std::byte> MmapFile::bytes() noexcept {
    if (!valid()) {
        return {};
    }

    return {data_, size_};
}

std::span<const std::byte> MmapFile::bytes() const noexcept {
    if (!valid()) {
        return {};
    }

    return {data_, size_};
}

const std::filesystem::path& MmapFile::path() const noexcept {
    return path_;
}

Status MmapFile::flush() noexcept {
    if (!valid()) {
        return Status{StatusCode::invalid_argument, invalid_file_detail};
    }

#if defined(_WIN32)
    return Status{StatusCode::io_error, unsupported_detail};
#else
    if (::msync(data_, size_, MS_SYNC) == -1) {
        return Status{StatusCode::io_error, flush_failed_detail};
    }

    return Status::ok();
#endif
}

Status MmapFile::resize(std::size_t new_size) {
    if (new_size == 0) {
        return invalid_size_status();
    }
    if (!valid()) {
        return Status{StatusCode::invalid_argument, invalid_file_detail};
    }

#if defined(_WIN32)
    static_cast<void>(new_size);
    return Status{StatusCode::io_error, unsupported_detail};
#else
    const Status flush_status = flush();
    if (!flush_status.is_ok()) {
        return flush_status;
    }

    std::byte* old_data = data_;
    const std::size_t old_size = size_;
    data_ = nullptr;
    size_ = 0;

    if (::munmap(old_data, old_size) == -1) {
        return Status{StatusCode::io_error, unmap_failed_detail};
    }

    if (::ftruncate(fd_, static_cast<off_t>(new_size)) == -1) {
        return Status{StatusCode::io_error, truncate_failed_detail};
    }

    void* mapping = ::mmap(nullptr, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (mapping == MAP_FAILED) {
        return Status{StatusCode::io_error, map_failed_detail};
    }

    data_ = static_cast<std::byte*>(mapping);
    size_ = new_size;
    return Status::ok();
#endif
}

Status MmapFile::close() noexcept {
    Status first_error = Status::ok();

#if defined(_WIN32)
    reset();
    return Status::ok();
#else
    if (valid() && options_.flush_on_destroy) {
        const Status flush_status = flush();
        if (!flush_status.is_ok()) {
            first_error = flush_status;
        }
    }

    if (data_ != nullptr && size_ != 0) {
        if (::munmap(data_, size_) == -1 && first_error.is_ok()) {
            first_error = Status{StatusCode::io_error, unmap_failed_detail};
        }
    }

    if (fd_ != -1) {
        if (::close(fd_) == -1 && first_error.is_ok()) {
            first_error = Status{StatusCode::io_error, close_failed_detail};
        }
    }

    reset();
    return first_error;
#endif
}

void MmapFile::reset() noexcept {
    fd_ = -1;
    data_ = nullptr;
    size_ = 0;
    path_.clear();
    options_ = {};
}

} // namespace aether::io
