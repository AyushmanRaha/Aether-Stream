#include <aether/core/status.hpp>
#include <aether/io/mmap_file.hpp>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <unistd.h>
#include <utility>

namespace {

int check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return 1;
    }

    return 0;
}

void write_marker(std::span<std::byte> bytes, std::size_t offset, const char* marker) {
    std::memcpy(bytes.data() + offset, marker, std::strlen(marker));
}

bool marker_matches(std::span<const std::byte> bytes, std::size_t offset, const char* marker) {
    return std::memcmp(bytes.data() + offset, marker, std::strlen(marker)) == 0;
}

std::filesystem::path unique_path(const char* suffix) {
    const auto name = std::string{"aether_mmap_file_test_"} + std::to_string(::getpid()) + suffix;
    return std::filesystem::temp_directory_path() / name;
}

} // namespace

int main() {
    int failures = 0;

    const auto path = unique_path(".mmap");
    const auto destructor_path = unique_path("_destructor.mmap");
    std::filesystem::remove(path);
    std::filesystem::remove(destructor_path);

    const auto empty_path_result = aether::io::MmapFile::create({}, 4096);
    failures += check(!empty_path_result.has_value(), "create should reject empty path");
    failures += check(empty_path_result.status().code() == aether::StatusCode::invalid_argument,
                      "empty path should return invalid_argument");

    const auto zero_size_result = aether::io::MmapFile::create(path, 0);
    failures += check(!zero_size_result.has_value(), "create should reject zero size");
    failures += check(zero_size_result.status().code() == aether::StatusCode::invalid_argument,
                      "zero size should return invalid_argument");

    auto create_result = aether::io::MmapFile::create(path, 4096);
    failures += check(create_result.has_value(), "create should succeed");
    if (!create_result.has_value()) {
        return 1;
    }

    auto file = std::move(create_result).value();
    failures += check(file.valid(), "created file should be valid");
    failures += check(file.size() == 4096, "created file size should be 4096");
    failures += check(file.data() != nullptr, "created file data should not be null");
    failures += check(file.bytes().size() == 4096, "created file byte span should match size");

    constexpr const char* marker = "Aether mmap persistence marker";
    write_marker(file.bytes(), 0, marker);
    failures += check(file.flush().is_ok(), "flush after write should succeed");
    failures += check(file.close().is_ok(), "close after flush should succeed");
    failures += check(!file.valid(), "closed file should be invalid");

    auto reopen_result = aether::io::MmapFile::open_existing(path);
    failures += check(reopen_result.has_value(), "open_existing should succeed");
    if (!reopen_result.has_value()) {
        return 1;
    }

    auto reopened = std::move(reopen_result).value();
    failures +=
        check(marker_matches(reopened.bytes(), 0, marker), "reopened bytes should match marker");

    aether::io::MmapFile moved{std::move(reopened)};
    failures += check(!reopened.valid(), "move constructor should invalidate source");
    failures += check(moved.valid(), "move constructor should preserve destination validity");
    failures += check(marker_matches(moved.bytes(), 0, marker), "moved file should expose marker");

    constexpr const char* prefix = "resize prefix";
    write_marker(moved.bytes(), 64, prefix);
    failures += check(moved.resize(8192).is_ok(), "resize to 8192 should succeed");
    failures += check(moved.size() == 8192, "resized file size should be 8192");
    failures += check(marker_matches(moved.bytes(), 64, prefix), "resize should preserve prefix");

    constexpr const char* new_region = "new region marker";
    write_marker(moved.bytes(), 5000, new_region);
    failures += check(moved.flush().is_ok(), "flush after resize should succeed");
    failures += check(moved.close().is_ok(), "close after resize should succeed");

    constexpr const char* destructor_marker = "destructor flush marker";
    {
        auto destructor_result = aether::io::MmapFile::create(destructor_path, 4096);
        failures += check(destructor_result.has_value(), "destructor test create should succeed");
        if (!destructor_result.has_value()) {
            return 1;
        }
        auto destructor_file = std::move(destructor_result).value();
        write_marker(destructor_file.bytes(), 128, destructor_marker);
    }

    auto destructor_reopen_result = aether::io::MmapFile::open_existing(destructor_path);
    failures +=
        check(destructor_reopen_result.has_value(), "destructor-flushed file should reopen");
    if (!destructor_reopen_result.has_value()) {
        return 1;
    }
    auto destructor_reopened = std::move(destructor_reopen_result).value();
    failures += check(marker_matches(destructor_reopened.bytes(), 128, destructor_marker),
                      "destructor should flush marker");
    failures += check(destructor_reopened.close().is_ok(), "destructor test close should succeed");

    std::filesystem::remove(path);
    std::filesystem::remove(destructor_path);

    if (failures != 0) {
        return 1;
    }

    std::cout << "mmap file test passed\n";
    return 0;
}
