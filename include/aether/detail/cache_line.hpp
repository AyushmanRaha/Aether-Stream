#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

namespace aether::detail {

inline constexpr std::size_t cache_line_size = 64;

template <typename T> struct alignas(cache_line_size) CachePadded {
    T value;

    constexpr CachePadded() noexcept(std::is_nothrow_default_constructible_v<T>) : value{} {}

    template <typename... Args>
    explicit constexpr CachePadded(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : value(std::forward<Args>(args)...) {}

    constexpr T& get() noexcept {
        return value;
    }
    constexpr const T& get() const noexcept {
        return value;
    }
};

static_assert(cache_line_size >= alignof(std::max_align_t));

} // namespace aether::detail
