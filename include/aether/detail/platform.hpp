#pragma once

namespace aether::detail {

#if defined(__clang__)
inline constexpr bool compiler_clang = true;
#else
inline constexpr bool compiler_clang = false;
#endif

#if defined(__GNUC__) && !defined(__clang__)
inline constexpr bool compiler_gcc = true;
#else
inline constexpr bool compiler_gcc = false;
#endif

#if defined(_MSC_VER)
inline constexpr bool compiler_msvc = true;
#else
inline constexpr bool compiler_msvc = false;
#endif

#if defined(__APPLE__) && defined(__MACH__)
inline constexpr bool platform_macos = true;
#else
inline constexpr bool platform_macos = false;
#endif

#if defined(__linux__)
inline constexpr bool platform_linux = true;
#else
inline constexpr bool platform_linux = false;
#endif

#if defined(_WIN32)
inline constexpr bool platform_windows = true;
#else
inline constexpr bool platform_windows = false;
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
inline constexpr bool arch_arm64 = true;
#else
inline constexpr bool arch_arm64 = false;
#endif

#if defined(__x86_64__) || defined(_M_X64)
inline constexpr bool arch_x86_64 = true;
#else
inline constexpr bool arch_x86_64 = false;
#endif

#if defined(_MSC_VER)
#define AETHER_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define AETHER_FORCE_INLINE inline __attribute__((always_inline))
#else
#define AETHER_FORCE_INLINE inline
#endif

} // namespace aether::detail
