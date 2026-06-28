#include <aether/utils/cpu_affinity.hpp>
#include <thread>

#if defined(__linux__)
#include <pthread.h>
#include <sched.h>
#endif

namespace aether::utils {
namespace {

[[nodiscard]] std::uint32_t hardware_threads() noexcept {
    return static_cast<std::uint32_t>(std::thread::hardware_concurrency());
}

[[nodiscard]] bool valid_cpu_index(std::uint32_t cpu_index) noexcept {
    const std::uint32_t threads = hardware_threads();
    return threads == 0 || cpu_index < threads;
}

} // namespace

CpuAffinityInfo cpu_affinity_info() noexcept {
#if defined(__linux__)
    return CpuAffinityInfo{true, hardware_threads(), "linux"};
#elif defined(__APPLE__)
    return CpuAffinityInfo{false, hardware_threads(), "macos"};
#else
    return CpuAffinityInfo{false, hardware_threads(), "unsupported"};
#endif
}

bool cpu_affinity_supported() noexcept {
    return cpu_affinity_info().supported;
}

Status pin_current_thread_to_cpu(std::uint32_t cpu_index) noexcept {
    if (!valid_cpu_index(cpu_index)) {
        return Status{StatusCode::invalid_argument, "cpu index exceeds hardware thread count"};
    }

#if defined(__linux__)
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu_index, &set);
    if (pthread_setaffinity_np(pthread_self(), sizeof(set), &set) != 0) {
        return Status{StatusCode::io_error, "pthread_setaffinity_np failed"};
    }
#endif
    return Status::ok();
}

Status clear_current_thread_affinity() noexcept {
#if defined(__linux__)
    const std::uint32_t threads = hardware_threads();
    if (threads == 0) {
        return Status::ok();
    }
    cpu_set_t set;
    CPU_ZERO(&set);
    for (std::uint32_t cpu = 0; cpu < threads; ++cpu) {
        CPU_SET(cpu, &set);
    }
    if (pthread_setaffinity_np(pthread_self(), sizeof(set), &set) != 0) {
        return Status{StatusCode::io_error, "pthread_setaffinity_np failed"};
    }
#endif
    return Status::ok();
}

} // namespace aether::utils
