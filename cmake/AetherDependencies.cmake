include_guard(GLOBAL)

# Phase 1 intentionally has no external dependencies.
# The first test is a tiny CTest executable with a custom main.
#
# Future phases may add dependency setup here, for example:
# - GoogleTest for richer unit tests
# - Google Benchmark for performance benchmarks
#
# Keep third-party dependency logic isolated in this file.

find_package(Threads REQUIRED)
