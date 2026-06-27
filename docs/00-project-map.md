# Project Map

## Current status: completed through Phase 4

Aether-Stream currently has the Phase 0 through Phase 4 foundation in place: repository setup, a real CMake library/test/example pipeline, core public types, status/error handling, an expected-like result wrapper, configuration structs, a non-owning message model, a header-only SPSC ring buffer, SPSC correctness hardening, utility helpers, examples, CTest coverage, and a manual SPSC stress tool.

The repository is still not a complete broker. It does not yet include persistence, a broker API, measured benchmark results, a CLI toolkit, metrics/diagnostics, CI automation, packaging, or production-readiness claims.

## Current repository layout

- `README.md` explains the project goal, current Phase 4 status, build/test commands, examples, and explicit non-goals for the current implementation.
- `LICENSE` contains the MIT license for the project.
- `.gitignore` excludes local build outputs, logs, persistence files, cache files, and editor artifacts.
- `.editorconfig` keeps whitespace, line endings, and indentation consistent across editors.
- `.clang-format` defines the C++ formatting style.
- `AGENTS.md` gives coding agents concise current context, build targets, phase boundaries, and no-overclaim rules.
- `.github/CODEOWNERS` contains repository ownership metadata only; there is no GitHub Actions CI setup yet.
- `cmake/` contains reusable CMake modules.
- `include/aether/` contains public library headers.
- `src/` contains compiled library implementation files.
- `examples/` contains small usage examples.
- `tests/` contains standalone CTest executables without GoogleTest.
- `tools/` contains the manual SPSC stress-validation executable.
- `scripts/` contains local setup, test, and formatting scripts.
- `docs/` contains this project map, the learning roadmap, the SPSC ring-buffer design document, and the memory-ordering document.


## Current docs

- `docs/00-project-map.md`: current repository map, build targets, implemented components, and phase boundaries.
- `docs/01-learning-roadmap.md`: study path for contributors learning the project phase by phase.
- `docs/ring-buffer-design.md`: Phase 3-4 SPSC ring-buffer design, API, slot lifecycle, tests, limitations, and future work.
- `docs/memory-ordering.md`: Phase 4 acquire/release memory-ordering protocol for `SpscRingBuffer<T, Capacity>`.

## Build system

The top-level `CMakeLists.txt` configures a C++20 project and defines these current targets when their options are enabled:

- `aether_stream`: main library target.
- `aether::stream`: public alias target for consumers.
- `aether_smoke`: smoke example executable.
- `aether_basic_spsc`: basic SPSC example executable.
- `aether_stress_spsc`: manual SPSC stress tool.
- `aether_test_version`: version CTest executable.
- `aether_test_status`: status CTest executable.
- `aether_test_message`: message CTest executable.
- `aether_test_spsc_basic`: basic SPSC CTest executable.
- `aether_test_spsc_wraparound`: SPSC wraparound CTest executable.
- `aether_test_spsc_concurrent`: concurrent ordered-transfer CTest executable.
- `aether_test_spsc_move_only`: move-only payload CTest executable.
- `aether_test_spsc_stress`: multi-capacity SPSC stress CTest executable.

Reusable CMake modules are:

- `cmake/AetherOptions.cmake`: developer-mode defaults and build options for tests, examples, tools, benchmark placeholder wiring, warnings, and warnings-as-errors.
- `cmake/AetherCompilerWarnings.cmake`: project warning flags for selected targets.
- `cmake/AetherDependencies.cmake`: centralized dependency setup; it currently finds only threads and does not fetch third-party packages.

`AETHER_BUILD_BENCHMARKS` is intentionally a Phase 5 placeholder. Turning it on does not add benchmark dependencies or benchmark targets.

## Public headers

- `include/aether/version.hpp` exposes version constants and `version_string()`.
- `include/aether/core/types.hpp` defines stable core aliases, constants, and a power-of-two helper.
- `include/aether/core/status.hpp` declares status codes and the lightweight `Status` type.
- `include/aether/core/expected.hpp` provides a small C++20 expected-like result wrapper.
- `include/aether/core/config.hpp` defines queue, WAL, and broker configuration structs with validation helpers, including queue tuning fields used by the current SPSC work.
- `include/aether/message.hpp` defines the non-owning message header/view model and validation helpers.
- `include/aether/spsc_ring_buffer.hpp` defines the header-only `SpscRingBuffer<T, Capacity>` for exactly one producer and exactly one consumer.
- `include/aether/detail/cache_line.hpp` provides cache-line padding/alignment helpers.
- `include/aether/detail/platform.hpp` provides platform/compiler/architecture detection and a force-inline macro.
- `include/aether/utils/clock.hpp` provides a monotonic nanosecond clock and stopwatch helper.
- `include/aether/utils/thread_utils.hpp` provides CPU relax, yielding, spin-wait, and best-effort current-thread naming helpers.

## Library implementation files

- `src/version.cpp` compiles the version API into the library target.
- `src/core/status.cpp` compiles stable status names and default messages into the library target.

## Examples

- `examples/smoke.cpp` links against `aether::stream` and prints the library version.
- `examples/basic_spsc.cpp` demonstrates integer queue usage and queueing `MessageHeader` values.

## Tests

CTest currently covers:

- version constants and `version_string()`;
- status/error handling and expected-like result behavior;
- message view construction and validation;
- basic SPSC push/pop, empty/full behavior, FIFO order, and `MessageHeader` transfer;
- SPSC wraparound and rolling push/pop behavior;
- concurrent SPSC transfer of at least 1,000,000 ordered values between one producer and one consumer;
- move-only payload support, including `std::unique_ptr<int>` and a custom move-only type;
- SPSC stress coverage for capacities `64`, `256`, `1024`, and `65536`.

## Tools

- `tools/stress_spsc.cpp` builds as `aether_stress_spsc` when `AETHER_BUILD_TOOLS=ON`.
- The stress tool validates ordered transfer through the SPSC queue for selected capacities and message counts.
- The stress tool is for manual correctness/stress validation only. Its output must not be presented as benchmark results.

## Scripts

- `scripts/bootstrap_macos.sh` checks for common macOS development tools and creates local build directories.
- `scripts/run_tests.sh` configures, builds, and runs the local CTest suite.
- `scripts/format_all.sh` formats or checks C/C++ files in known source directories.

## What does not exist yet

Phase 5 and later work is not implemented yet. The repository currently has no:

- benchmark framework or measured benchmark-results document;
- benchmark dependency such as Google Benchmark;
- mmap-backed file layer;
- WAL writer, reader, recovery, or file-format implementation;
- broker API or broker runtime;
- CLI toolkit apps;
- metrics or diagnostics subsystem;
- GitHub Actions CI, sanitizer jobs, packaging, export/install logic, or release automation;
- production-ready, HFT-ready, or latency-performance claims.

## Next phase

Phase 5 is the next planned phase: benchmark framework and honest performance reporting. That phase should add measurement infrastructure and reporting only when explicitly requested, and it must avoid fake or marketing-style performance claims.

## Phase boundaries

- Phase 0 completed: baseline repository setup, README, license, ignore/editor/format files, initial docs, and macOS bootstrap script.
- Phase 1 completed: real CMake project, library target, alias target, public version API, smoke example, first CTest path, test script, and formatting script.
- Phase 2 completed: core public types, status/error handling, expected-like wrapper, configuration structs, message model, and status/message tests.
- Phase 3 completed: header-only SPSC ring buffer v1, cache-line/platform helpers, basic SPSC example, and basic/wraparound SPSC tests.
- Phase 4 completed: SPSC correctness hardening, move support, approximate sizing, acquire/release memory-order documentation, queue tuning fields, utility helpers, concurrent/move-only/stress tests, and manual stress tool.
- Phase 5 next: benchmark framework and honest performance reporting, only when explicitly requested.
- Phase 6+ later: persistence, WAL, broker behavior, CLI toolkit, metrics/diagnostics, CI, packaging, release work, and advanced tuning.
