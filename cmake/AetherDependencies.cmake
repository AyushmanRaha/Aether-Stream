include_guard(GLOBAL)

# Keep third-party dependency logic isolated in this file.
# Google Benchmark is only resolved when Phase 5 benchmark targets are enabled.

find_package(Threads REQUIRED)

if(AETHER_BUILD_BENCHMARKS)
  find_package(benchmark CONFIG QUIET)

  if(NOT benchmark_FOUND)
    include(FetchContent)

    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
      googlebenchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG v1.9.5
    )

    FetchContent_MakeAvailable(googlebenchmark)
  endif()
endif()
