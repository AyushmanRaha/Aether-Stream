include_guard(GLOBAL)

# Phase 4 intentionally has no external dependencies beyond platform threads.
# Current tests are tiny CTest executables with custom mains.
#
# Future phases may add dependency setup here only when explicitly requested,
# for example Google Benchmark during Phase 5 benchmark work.
#
# Keep third-party dependency logic isolated in this file.

find_package(Threads REQUIRED)
