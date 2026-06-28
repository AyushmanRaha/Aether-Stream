#pragma once

#include <aether/core/status.hpp>
#include <cstdint>
#include <string_view>

namespace aether::utils {

struct CpuAffinityInfo {
    bool supported{};
    std::uint32_t hardware_threads{};
    std::string_view platform{};
};

[[nodiscard]] CpuAffinityInfo cpu_affinity_info() noexcept;
[[nodiscard]] bool cpu_affinity_supported() noexcept;
[[nodiscard]] Status pin_current_thread_to_cpu(std::uint32_t cpu_index) noexcept;
[[nodiscard]] Status clear_current_thread_affinity() noexcept;

} // namespace aether::utils
