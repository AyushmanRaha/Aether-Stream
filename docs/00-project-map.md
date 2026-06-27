# Project Map

## Current status: completed through Phase 9

Aether-Stream currently has the Phase 0 through Phase 9 foundation in place: repository setup, a real CMake library/test/example pipeline, core public types, status/error handling, an expected-like result wrapper, configuration structs, a non-owning message model, a header-only SPSC ring buffer, SPSC correctness hardening, utility helpers, examples, CTest coverage, a manual SPSC stress tool, Google Benchmark wiring, benchmark executables, a benchmark runner, benchmark methodology/performance-results docs, a POSIX memory-mapped file abstraction, append-only WAL writer/reader support, an in-memory broker API, a WAL-backed persistent broker API, and typed replay for trivially copyable persistent broker event types, and Phase 9 CLI apps.

The repository now includes the first developer-facing broker layer. It is still not production-ready and does not yet include metrics/diagnostics, CI automation, packaging, advanced low-latency APIs, or official measured benchmark results.

## Current repository layout

- `README.md` explains the project goal, current Phase 9 status, build/test/benchmark commands, examples, and explicit non-goals for the current implementation.
- `LICENSE` contains the MIT license for the project.
- `.gitignore` excludes local build outputs, logs, persistence files, cache files, and editor artifacts.
- `.editorconfig` keeps whitespace, line endings, and indentation consistent across editors.
- `.clang-format` defines the C++ formatting style.
- `AGENTS.md` gives coding agents concise current context, build targets, phase boundaries, and no-overclaim rules.
- `.github/CODEOWNERS` contains repository ownership metadata only; there is no GitHub Actions CI setup yet.
- `cmake/` contains reusable CMake modules.
- `include/aether/` contains public library headers.
- `apps/` contains Phase 9 CLI demo applications.
- `src/` contains compiled library implementation files.
- `examples/` contains small usage examples.
- `benchmarks/` contains Google Benchmark-based Phase 5 SPSC benchmark executables.
- `tests/` contains standalone CTest executables without GoogleTest.
- `tools/` contains the manual SPSC stress-validation executable.
- `scripts/` contains local setup, test, and formatting scripts.
- `docs/` contains this project map, the learning roadmap, SPSC design docs, broker API docs, benchmark docs, mmap notes, and WAL format notes.

## Current docs

- `docs/00-project-map.md`: current repository map, build targets, implemented components, and phase boundaries.
- `docs/01-learning-roadmap.md`: study path for contributors learning the project phase by phase.
- `docs/ring-buffer-design.md`: Phase 3-4 SPSC ring-buffer design, API, slot lifecycle, tests, limitations, and future work.
- `docs/memory-ordering.md`: Phase 4 acquire/release memory-ordering protocol for `SpscRingBuffer<T, Capacity>`.
- `docs/benchmark-methodology.md`: Phase 5 benchmark scope, build mode, runner workflow, raw-output policy, and limitations.
- `docs/performance-results.md`: template for measured performance results; no official measured numbers are committed yet.
- `docs/mmap-notes.md`: Phase 6 notes for mmap behavior, `MmapFile` lifecycle, POSIX scope, and non-goals.
- `docs/wal-format.md`: Phase 7 WAL record format, checksum policy, reader behavior, Phase 8 persistent broker integration note, and limitations.
- `docs/broker-api.md`: Phase 8 broker API guide, including in-memory broker usage, persistent broker usage, WAL-before-queue durability semantics, typed replay, SPSC limitation, configuration, and current limitations.
- `docs/cli-guide.md`: Phase 9 CLI app guide and demo flow.

## Build system

The top-level `CMakeLists.txt` configures a C++20 project and defines these current targets when their options are enabled:

- `aether_stream`: main library target.
- `aether::stream`: public alias target for consumers.
- `aether_smoke`: smoke example executable.
- `aether_basic_spsc`: basic SPSC example executable.
- `aether_mmap_smoke`: mmap smoke example executable.
- `aether_wal_replay`: WAL replay example executable.
- `aether_broker_basic`: in-memory broker example executable.
- `aether_persistent_broker`: WAL-backed persistent broker example executable.
- `aether_stress_spsc`: manual SPSC stress tool.
- `aether_app_bench`: CLI demo benchmark target, output executable `aether_bench`, emitted under `${CMAKE_BINARY_DIR}/apps`.
- `aether_app_pub`: CLI WAL publisher demo target, output executable `aether_pub`, emitted under `${CMAKE_BINARY_DIR}/apps`.
- `aether_app_sub`: CLI local subscriber / typed WAL replay demo target, output executable `aether_sub`, emitted under `${CMAKE_BINARY_DIR}/apps`.
- `aether_app_replay`: CLI raw WAL replay target, output executable `aether_replay`, emitted under `${CMAKE_BINARY_DIR}/apps`.
- `aether_app_inspect_wal`: CLI WAL inspection target, output executable `aether_inspect_wal`, emitted under `${CMAKE_BINARY_DIR}/apps`.
- `aether_test_version`: version CTest executable.
- `aether_test_status`: status CTest executable.
- `aether_test_message`: message CTest executable.
- `aether_test_spsc_basic`: basic SPSC CTest executable.
- `aether_test_spsc_wraparound`: SPSC wraparound CTest executable.
- `aether_test_spsc_concurrent`: concurrent ordered-transfer CTest executable.
- `aether_test_spsc_move_only`: move-only payload CTest executable.
- `aether_test_spsc_stress`: multi-capacity SPSC stress CTest executable.
- `aether_test_mmap_file`: mmap file CTest executable.
- `aether_test_wal_record`: WAL record-format CTest executable.
- `aether_test_wal_writer`: WAL writer CTest executable.
- `aether_test_wal_reader`: WAL reader CTest executable.
- `aether_test_broker`: in-memory broker CTest executable.
- `aether_test_persistent_broker`: persistent broker CTest executable.
- `aether_test_cli_args`: CLI argument parsing CTest executable.
- `aether_bench_spsc_throughput`: SPSC throughput benchmark executable.
- `aether_bench_spsc_latency`: timestamped SPSC latency benchmark executable.
- `aether_bench_payload_sizes`: SPSC payload-size comparison benchmark executable.

Reusable CMake modules are:

- `cmake/AetherOptions.cmake`: developer-mode defaults and build options for tests, examples, tools, CLI apps, benchmarks, warnings, and warnings-as-errors.
- `cmake/AetherCompilerWarnings.cmake`: project warning flags for selected targets.
- `cmake/AetherDependencies.cmake`: centralized dependency setup; it always finds threads and resolves Google Benchmark only when `AETHER_BUILD_BENCHMARKS=ON`.

`AETHER_BUILD_APPS` enables the Phase 9 CLI app targets and emits them under `${CMAKE_BINARY_DIR}/apps`.

`AETHER_BUILD_BENCHMARKS` enables the Phase 5 Google Benchmark dependency wiring and benchmark targets. Benchmark executables are separate from CTest targets and are emitted under `${CMAKE_BINARY_DIR}/benchmarks`.

## Public headers

- `include/aether/version.hpp` exposes version constants and `version_string()`.
- `include/aether/core/types.hpp` defines stable core aliases, constants, and a power-of-two helper.
- `include/aether/core/status.hpp` declares status codes and the lightweight `Status` type.
- `include/aether/core/expected.hpp` provides a small C++20 expected-like result wrapper.
- `include/aether/core/config.hpp` defines queue, WAL, and broker configuration structs with validation helpers.
- `include/aether/cli/args.hpp` declares Phase 9 CLI option structs, parsers, and help text accessors.
- `include/aether/message.hpp` defines the non-owning message header/view model and validation helpers.
- `include/aether/broker.hpp` declares the Phase 8 in-memory broker API over the SPSC queue.
- `include/aether/persistent_broker.hpp` declares the Phase 8 WAL-backed persistent broker API and typed replay helper for trivially copyable event types.
- `include/aether/io/mmap_file.hpp` declares the Phase 6 RAII memory-mapped file wrapper.
- `include/aether/wal/record.hpp` defines the Phase 7 WAL record header, record view, and explicit 40-byte serialization helpers.
- `include/aether/wal/checksum.hpp` declares CRC32 and WAL record checksum helpers.
- `include/aether/wal/wal_writer.hpp` declares the append-only WAL writer.
- `include/aether/wal/wal_reader.hpp` declares the sequential WAL reader and replay helper.
- `include/aether/spsc_ring_buffer.hpp` defines the header-only `SpscRingBuffer<T, Capacity>` for exactly one producer and exactly one consumer.
- `include/aether/detail/cache_line.hpp` provides cache-line padding/alignment helpers.
- `include/aether/detail/platform.hpp` provides platform/compiler/architecture detection and a force-inline macro.
- `include/aether/utils/clock.hpp` provides a monotonic nanosecond clock and stopwatch helper.
- `include/aether/utils/thread_utils.hpp` provides CPU relax, yielding, spin-wait, and best-effort current-thread naming helpers.

## Library implementation files

- `src/version.cpp` compiles the version API into the library target.
- `src/core/status.cpp` compiles stable status names and default messages into the library target.
- `src/broker.cpp` implements broker durability mode naming and broker/PersistentBroker configuration validation helpers.
- `src/cli/args.cpp` implements dependency-free Phase 9 CLI argument parsing.
- `src/io/mmap_file.cpp` compiles the POSIX mmap implementation into the library target.
- `src/wal/checksum.cpp` implements CRC32 and WAL record checksum helpers.
- `src/wal/wal_writer.cpp` implements the append-only mmap-backed WAL writer.
- `src/wal/wal_reader.cpp` implements the sequential mmap-backed WAL reader.

## Apps

- `apps/aether_bench.cpp` builds the local CLI demo benchmark.
- `apps/aether_pub.cpp` writes demo `OrderEvent` records to WAL.
- `apps/aether_sub.cpp` runs a local subscriber demo or typed WAL replay.
- `apps/aether_replay.cpp` prints raw WAL record summaries.
- `apps/aether_inspect_wal.cpp` scans and summarizes WAL files.

## Examples

- `examples/smoke.cpp` links against `aether::stream` and prints the library version.
- `examples/basic_spsc.cpp` demonstrates integer queue usage and queueing `MessageHeader` values.
- `examples/mmap_smoke.cpp` demonstrates creating, writing, flushing, closing, and reopening a mapped file.
- `examples/wal_replay.cpp` demonstrates writing three WAL records and replaying them sequentially.
- `examples/broker_basic.cpp` demonstrates publishing and consuming an event through `aether::Broker`.
- `examples/persistent_broker.cpp` demonstrates WAL-backed publish, consume, flush, and typed replay through `aether::PersistentBroker`.

## Tests

CTest currently covers:

- version constants and `version_string()`;
- status/error handling and expected-like result behavior;
- message view construction and validation;
- basic SPSC push/pop, empty/full behavior, FIFO order, and `MessageHeader` transfer;
- SPSC wraparound and rolling push/pop behavior;
- concurrent SPSC transfer of at least 1,000,000 ordered values between one producer and one consumer;
- move-only payload support, including `std::unique_ptr<int>` and a custom move-only type;
- SPSC stress coverage for capacities `64`, `256`, `1024`, and `65536`;
- mmap create/write/flush/close/reopen, move ownership, resize, and destructor-flush behavior;
- WAL record serialization, reserved-byte determinism, header validation, and checksum validation;
- WAL writer creation, append offsets, sequence assignment, out-of-space handling, flush, and zero-length payloads;
- WAL reader sequential replay, reset, visitor replay, zero-filled tail EOF, partial-record clean stop, and checksum corruption detection;
- in-memory broker publish/consume, full/empty behavior, FIFO order, `try_emplace`, move-only payload support, and runtime queue capacity validation;
- persistent broker open/config validation, WAL-before-queue behavior, WAL record readability, full-queue no-append behavior, flush, and typed replay;
- CLI argument parsing defaults, help flags, `--key value`, `--key=value`, valid options, and invalid argument handling in `tests/test_cli_args.cpp`.

## Benchmarks

- `benchmarks/bench_spsc_throughput.cpp` measures ordered SPSC throughput for capacities 64, 256, 1024, and 65536.
- `benchmarks/bench_spsc_latency.cpp` measures approximate timestamped SPSC transfer latency for capacities 1024 and 65536.
- `benchmarks/bench_payload_sizes.cpp` compares payload objects sized 8B, 32B, 64B, 256B, and 1024B for capacities 1024 and 65536.
- `scripts/run_benchmarks.sh` configures a Release build, runs CTest, runs all benchmarks, and stores text/JSON outputs plus environment metadata under `benchmark-results/YYYYMMDD-HHMMSS/`.
- The performance-results document is a template until measured outputs are intentionally copied from raw result files.

## Tools

- `tools/stress_spsc.cpp` builds as `aether_stress_spsc` when `AETHER_BUILD_TOOLS=ON`.
- The stress tool validates ordered transfer through the SPSC queue for selected capacities and message counts.
- The stress tool is for manual correctness/stress validation only. Its output must not be presented as benchmark results.

## Scripts

- `scripts/bootstrap_macos.sh` checks for common macOS development tools and creates local build directories.
- `scripts/run_tests.sh` configures, builds, and runs the local CTest suite.
- `scripts/run_benchmarks.sh` configures a Release benchmark build, runs CTest, and captures raw benchmark outputs.
- `scripts/format_all.sh` formats or checks C/C++ files in known source directories.

## What does not exist yet

The following future phases are not implemented yet:

- official measured benchmark-results numbers committed from a controlled run;
- metrics or diagnostics subsystem;
- GitHub Actions CI, sanitizer jobs, packaging, export/install logic, or release automation;
- advanced low-latency APIs such as batching, zero-copy reservation, and CPU affinity helpers;
- final portfolio/release documentation;
- production-ready, HFT-ready, or unsupported latency-performance claims.

## Next phase

Phase 10 is the next planned phase: metrics and diagnostics. Phase 9 added terminal apps, but the project still has no CI, packaging, networking, or production hardening.

## Phase boundaries

- Phase 0 completed: baseline repository setup, README, license, ignore/editor/format files, initial docs, and macOS bootstrap script.
- Phase 1 completed: real CMake project, library target, alias target, public version API, smoke example, first CTest path, test script, and formatting script.
- Phase 2 completed: core public types, status/error handling, expected-like wrapper, configuration structs, message model, and status/message tests.
- Phase 3 completed: header-only SPSC ring buffer v1, cache-line/platform helpers, basic SPSC example, and basic/wraparound SPSC tests.
- Phase 4 completed: SPSC correctness hardening, move support, approximate sizing, acquire/release memory-order documentation, queue tuning fields, utility helpers, concurrent/move-only/stress tests, and manual stress tool.
- Phase 5 completed: benchmark framework and honest performance reporting, including Google Benchmark wiring, SPSC benchmark executables, raw result runner, methodology doc, and performance-results template.
- Phase 6 completed: memory-mapped file abstraction, including `MmapFile`, POSIX mmap implementation, persistence tests, smoke example, and mmap notes.
- Phase 7 completed: WAL record format, CRC32 checksum support, append-only WAL writer, sequential WAL reader, replay example, tests, and WAL format documentation.
- Phase 8 completed: in-memory broker API, WAL-backed persistent broker API, WAL-before-queue durability semantics, typed replay for trivially copyable event types, broker examples, broker tests, and broker API documentation.
- Phase 9 completed: CLI toolkit and runnable terminal demonstrations.
- Phase 10+ later: metrics/diagnostics, CI, packaging, release work, and advanced tuning.
