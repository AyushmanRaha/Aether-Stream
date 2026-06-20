#pragma once

#include <string_view>

namespace aether {

inline constexpr int version_major = 0;
inline constexpr int version_minor = 1;
inline constexpr int version_patch = 0;

std::string_view version_string() noexcept;

} // namespace aether
