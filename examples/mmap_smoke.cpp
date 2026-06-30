// Creates and exercises a memory-mapped file to confirm MmapFile behavior.
#include <aether/io/mmap_file.hpp>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <utility>

int main() {
    const std::filesystem::path data_dir{"data"};
    const std::filesystem::path file_path = data_dir / "mmap_smoke.mmap";
    constexpr std::size_t file_size = 4096;
    constexpr const char* marker = "Aether-Stream mmap smoke record";

    std::error_code ec;
    std::filesystem::create_directories(data_dir, ec);
    if (ec) {
        std::cerr << "failed to create data directory\n";
        return 1;
    }

    auto create_result = aether::io::MmapFile::create(file_path, file_size);
    if (!create_result.has_value()) {
        std::cerr << "failed to create mmap file: " << create_result.status().message() << '\n';
        return 1;
    }

    auto file = std::move(create_result).value();
    std::memcpy(file.bytes().data(), marker, std::strlen(marker));

    if (!file.flush().is_ok()) {
        std::cerr << "failed to flush mmap file\n";
        return 1;
    }
    if (!file.close().is_ok()) {
        std::cerr << "failed to close mmap file\n";
        return 1;
    }

    auto reopen_result = aether::io::MmapFile::open_existing(file_path);
    if (!reopen_result.has_value()) {
        std::cerr << "failed to reopen mmap file: " << reopen_result.status().message() << '\n';
        return 1;
    }

    const auto reopened = std::move(reopen_result).value();
    if (std::memcmp(reopened.bytes().data(), marker, std::strlen(marker)) != 0) {
        std::cerr << "mmap smoke marker did not persist\n";
        return 1;
    }

    std::cout << "mmap smoke succeeded: " << file_path << '\n';
    return 0;
}
