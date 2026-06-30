# Project Map

## Current status: completed through Phase 13

Aether-Stream currently has the Phase 0 through Phase 13 foundation in place: repository setup, a real CMake library/test/example pipeline, core public types, status/error handling, an expected-like result wrapper, configuration structs, a non-owning message model, a header-only SPSC ring buffer, SPSC correctness hardening, utility helpers, examples, CTest coverage, a manual SPSC stress tool, Google Benchmark wiring, benchmark executables, a benchmark runner, benchmark methodology/performance-results docs, a POSIX memory-mapped file abstraction, append-only WAL writer/reader support, an in-memory broker API, a WAL-backed persistent broker API, typed replay for trivially copyable persistent broker event types, Phase 9 CLI apps, Phase 10 metrics/diagnostics, Phase 11 GitHub Actions CI, sanitizer/static-analysis workflows, benchmark smoke workflow, CMake install/export package support, Phase 12 batch broker, zero-copy SPSC, spin-wait utility, CPU affinity helper, low-latency benchmarks, low-latency tuning docs, HFT-style design notes, and Phase 13 portfolio documentation, inline Mermaid diagrams, limitations, interview notes, and v0.1.0 release notes.

The repository now includes the developer-facing broker layer, Phase 12 low-latency API additions, and Phase 13 final documentation/portfolio packaging. It is still not production-ready. CI, sanitizer jobs, clang-tidy integration, benchmark smoke checks, CMake package install/export support, and Phase 13 docs now exist. No official measured benchmark results have been committed.

## Current repository layout

- `README.md` is the Phase 13 portfolio front page with centered hero, badges, inline Mermaid diagrams, build/test/benchmark commands, docs links, and explicit non-goals.
- `LICENSE` contains the MIT license for the project.
- `.gitignore` excludes local build outputs, logs, persistence files, cache files, and editor artifacts.
- `.editorconfig` keeps whitespace, line endings, and indentation consistent across editors.
- `.clang-format` defines the C++ formatting style.
- `.clang-tidy` defines a moderate static-analysis configuration.
- `CONTRIBUTING.md` documents contributor build, test, format, sanitizer, and clang-tidy guidance.
- `CHANGELOG.md` tracks unreleased changes, including Phase 13 documentation packaging.
- `AGENTS.md` gives coding agents concise Phase 13-complete context, build targets, phase boundaries, Mermaid diagram guidance, and no-overclaim rules.
- `.github/CODEOWNERS` contains repository ownership metadata.
- `.github/workflows/` contains `ci.yml`, `sanitizer.yml`, and `benchmark-smoke.yml` for Phase 11 quality automation.
- `cmake/` contains reusable CMake modules, including Phase 11 sanitizer, install, and package config support.
- `include/aether/` contains public library headers, including `include/aether/metrics/` for Phase 10 metrics APIs and Phase 12 low-latency APIs.
- `apps/` contains Phase 9 CLI demo applications.
- `src/` contains compiled library implementation files, including `src/utils/cpu_affinity.cpp`.
- `examples/` contains small usage examples.
- `benchmarks/` contains Google Benchmark-based Phase 5 SPSC benchmarks, the Phase 10 broker end-to-end benchmark, and Phase 12 low-latency benchmarks.
- `tests/` contains standalone CTest executables without GoogleTest.
- `tools/` contains the manual SPSC stress-validation executable.
- `scripts/` contains local setup, test, and formatting scripts.
- `docs/` contains this project map, the learning roadmap, architecture docs, SPSC design docs, broker API docs, CLI docs, metrics docs, benchmark docs, mmap notes, WAL format notes, Phase 12 low-latency tuning and HFT-style design notes, limitations, interview notes, and the release checklist. README diagrams are inline Mermaid blocks rather than generated image assets.

## Current docs

- `docs/00-project-map.md`: current repository map, build targets, implemented components, and phase boundaries.
- `docs/01-learning-roadmap.md`: study path for contributors learning the project phase by phase.
- `docs/architecture.md`: layered architecture, Mermaid diagrams, data flow, metrics flow, build targets, and interview credibility notes.
- `docs/limitations.md`: explicit concurrency, persistence, deployment, benchmark, experimental API, and production-readiness limitations.
- `docs/interview-notes.md`: recruiter/interviewer pitch notes, deep-dive outline, tradeoffs, and Q&A.
- `RELEASE_NOTES_v0.1.0.md`: documentation-ready v0.1.0 candidate release notes.
- `docs/ring-buffer-design.md`: Phase 13-polished SPSC ring-buffer design, API, slot lifecycle, Mermaid diagram, tests, limitations, and interview explanation.
- `docs/memory-ordering.md`: acquire/release memory-ordering protocol for `SpscRingBuffer<T, Capacity>` with Mermaid sequence diagram and SPSC-only reasoning.
- `docs/benchmark-methodology.md`: final benchmark scope, canonical runner workflow, raw-output policy, environment metadata, platform caveats, and reporting prohibitions.
- `docs/performance-results.md`: publication-ready measured-results template; no official measured numbers are committed yet.
- `docs/mmap-notes.md`: Phase 6 notes for mmap behavior, `MmapFile` lifecycle, POSIX scope, and non-goals.
- `docs/wal-format.md`: final WAL record format spec, checksum policy, reader behavior, WAL-before-queue semantics, typed replay limits, and corruption/recovery definitions.
- `docs/broker-api.md`: broker API guide, including in-memory broker usage, persistent broker usage, WAL-before-queue durability semantics, typed replay, Phase 10 metrics APIs, SPSC limitation, configuration, and current limitations.
- `docs/cli-guide.md`: Phase 9 CLI app guide, demo flow, and Phase 10 CLI metrics output.
- `docs/metrics.md`: Phase 10 metrics, snapshots, WAL/recovery counters, and latency histogram guide.
- `docs/release-checklist.md`: pre-release verification checklist, including Phase 13 documentation checks.
- `docs/low-latency-tuning.md`: Phase 12 tuning notes for batching, zero-copy, spin waiting, CPU affinity, cache lines, and benchmark honesty.
- `docs/hft-design-notes.md`: Phase 12 HFT-style design/tradeoff notes, including SPSC scope, MPMC non-goal, WAL latency tradeoff, and real-HFT limitations.

Root-level project docs/config:

- `CONTRIBUTING.md`: contributor build, test, format, sanitizer, and clang-tidy guidance.
- `CHANGELOG.md`: unreleased change tracking.
- `.clang-tidy`: moderate static-analysis configuration.

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
- `aether_test_counters`: metrics counters CTest executable.
- `aether_test_latency_histogram`: latency histogram CTest executable.
- `aether_test_batch_broker`: batch broker CTest executable.
- `aether_test_zero_copy_spsc`: zero-copy SPSC CTest executable.
- `aether_bench_spsc_throughput`: SPSC throughput benchmark executable.
- `aether_bench_spsc_latency`: timestamped SPSC latency benchmark executable.
- `aether_bench_payload_sizes`: SPSC payload-size comparison benchmark executable.
- `aether_bench_broker_end_to_end`: broker end-to-end benchmark executable.
- `aether_bench_batch_publish`: batch publish benchmark executable.
- `aether_bench_zero_copy_spsc`: zero-copy SPSC benchmark executable.
- `aether_bench_spin_wait`: spin-wait microbenchmark executable.

Reusable CMake modules are:

- `cmake/AetherOptions.cmake`: developer-mode defaults and build options for tests, examples, tools, CLI apps, benchmarks, warnings, warnings-as-errors, ASAN, UBSAN, TSAN, clang-tidy, and install/export support.
- `cmake/AetherCompilerWarnings.cmake`: project warning flags for selected targets.
- `cmake/AetherDependencies.cmake`: centralized dependency setup; it always finds threads and resolves Google Benchmark only when `AETHER_BUILD_BENCHMARKS=ON`.
- `cmake/AetherSanitizers.cmake`: centralized ASAN/UBSAN/TSAN flag wiring.
- `cmake/AetherInstall.cmake`: install/export/package rule setup.
- `cmake/AetherStreamConfig.cmake.in`: installed package config template for `find_package(AetherStream CONFIG REQUIRED)`.

`AETHER_BUILD_APPS` enables the Phase 9 CLI app targets and emits them under `${CMAKE_BINARY_DIR}/apps`.

`AETHER_BUILD_BENCHMARKS` enables Google Benchmark dependency wiring plus the Phase 5, Phase 10, and Phase 12 benchmark targets. Benchmark executables are separate from CTest targets and are emitted under `${CMAKE_BINARY_DIR}/benchmarks`.

## Quality automation and packaging

Phase 11 adds repository-level quality automation:

- `.github/workflows/ci.yml` runs formatting checks, Ubuntu/macOS Debug and Release builds, CTest, clang-tidy, and package install smoke verification.
- `.github/workflows/sanitizer.yml` runs ASAN/UBSAN and TSAN builds on Ubuntu.
- `.github/workflows/benchmark-smoke.yml` builds benchmark targets and runs short smoke executions to catch benchmark breakage.
- `.clang-tidy` defines a moderate static-analysis profile.
- `cmake/AetherSanitizers.cmake` centralizes sanitizer flags.
- `cmake/AetherInstall.cmake` installs public headers and exports the package as `AetherStream`.
- `cmake/AetherStreamConfig.cmake.in` lets consumer projects use `find_package(AetherStream CONFIG REQUIRED)` and link `aether::stream`.

These checks improve maintainability and verification, but they do not make the project production-ready or create official benchmark claims.

## Public headers

- `include/aether/version.hpp` exposes version constants and `version_string()`.
- `include/aether/core/types.hpp` defines stable core aliases, constants, and a power-of-two helper.
- `include/aether/core/status.hpp` declares status codes and the lightweight `Status` type.
- `include/aether/core/expected.hpp` provides a small C++20 expected-like result wrapper.
- `include/aether/core/config.hpp` defines queue, WAL, and broker configuration structs with validation helpers.
- `include/aether/cli/args.hpp` declares Phase 9 CLI option structs, parsers, and help text accessors.
- `include/aether/metrics/snapshot.hpp` declares `BrokerMetricsSnapshot`.
- `include/aether/metrics/counters.hpp` declares relaxed-atomic `BrokerCounters`.
- `include/aether/metrics/latency_histogram.hpp` declares diagnostic `LatencyHistogram`.
- `include/aether/message.hpp` defines the non-owning message header/view model and validation helpers.
- `include/aether/broker.hpp` declares the Phase 8 in-memory broker API over the SPSC queue.
- `include/aether/batch_broker.hpp` declares the Phase 12 batch-oriented in-memory broker API.
- `include/aether/persistent_broker.hpp` declares the Phase 8 WAL-backed persistent broker API and typed replay helper for trivially copyable event types.
- `include/aether/io/mmap_file.hpp` declares the Phase 6 RAII memory-mapped file wrapper.
- `include/aether/wal/record.hpp` defines the Phase 7 WAL record header, record view, and explicit 40-byte serialization helpers.
- `include/aether/wal/checksum.hpp` declares CRC32 and WAL record checksum helpers.
- `include/aether/wal/wal_writer.hpp` declares the append-only WAL writer.
- `include/aether/wal/wal_reader.hpp` declares the sequential WAL reader and replay helper.
- `include/aether/spsc_ring_buffer.hpp` defines the header-only `SpscRingBuffer<T, Capacity>` for exactly one producer and exactly one consumer.
- `include/aether/zero_copy_spsc.hpp` defines the Phase 12 experimental zero-copy SPSC reservation API.
- `include/aether/detail/cache_line.hpp` provides cache-line padding/alignment helpers.
- `include/aether/detail/platform.hpp` provides platform/compiler/architecture detection and a force-inline macro.
- `include/aether/utils/clock.hpp` provides a monotonic nanosecond clock and stopwatch helper.
- `include/aether/utils/thread_utils.hpp` provides CPU relax, yielding, spin-wait, and best-effort current-thread naming helpers.
- `include/aether/utils/spin_wait.hpp` provides Phase 12 `cpu_relax` and `SpinWait` helpers.
- `include/aether/utils/cpu_affinity.hpp` declares Phase 12 CPU affinity information, support, pin, and clear helpers.

## Library implementation files

- `src/version.cpp` compiles the version API into the library target.
- `src/core/status.cpp` compiles stable status names and default messages into the library target.
- `src/broker.cpp` implements broker durability mode naming and broker/PersistentBroker configuration validation helpers.
- `src/cli/args.cpp` implements dependency-free Phase 9 CLI argument parsing.
- `src/metrics/latency_histogram.cpp` implements latency histogram statistics and percentile calculations.
- `src/utils/cpu_affinity.cpp` implements Linux-first CPU affinity helpers with safe unsupported-platform fallback behavior.
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
- CLI argument parsing defaults, help flags, `--key value`, `--key=value`, valid options, and invalid argument handling in `tests/test_cli_args.cpp`;
- metrics counters default/snapshot/reset/increment behavior, including WAL and recovery counter increments;
- latency histogram empty, single-sample, and multiple-sample stats, percentile clamping, clear, and reserve behavior;
- broker and persistent broker metrics integration as part of broker-oriented tests;
- batch broker batch ordering, partial batch behavior, empty/full edge cases, and invalid config behavior;
- zero-copy SPSC reservation, commit, cancel, destructor cancellation, active reservation guard, FIFO behavior, full behavior, wraparound, and move-only payload support.

## Benchmarks

- `benchmarks/bench_spsc_throughput.cpp` measures ordered SPSC throughput for capacities 64, 256, 1024, and 65536.
- `benchmarks/bench_spsc_latency.cpp` measures approximate timestamped SPSC transfer latency for capacities 1024 and 65536.
- `benchmarks/bench_payload_sizes.cpp` compares payload objects sized 8B, 32B, 64B, 256B, and 1024B for capacities 1024 and 65536.
- `benchmarks/bench_broker_end_to_end.cpp` measures local broker publish-to-consume paths with WAL disabled and enabled.
- `benchmarks/bench_batch_publish.cpp` compares single-message broker publish/consume patterns with batch broker publish/consume patterns.
- `benchmarks/bench_zero_copy_spsc.cpp` compares normal SPSC insertion with zero-copy reserve/construct/commit insertion.
- `benchmarks/bench_spin_wait.cpp` compares waiting primitive overhead.
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

The following planned or intentionally omitted work is not implemented yet:

- official measured benchmark-results numbers committed from a controlled run;
- networking or a live inter-process broker service;
- MPSC/MPMC queues;
- production-ready, HFT-ready, or unsupported latency/performance guarantees.

Phase 13 documentation and portfolio packaging is complete. Diagrams are inline Mermaid blocks in Markdown instead of generated image assets.

## Next phase

Phase 13 has been implemented. Future work should remain explicitly scoped and must preserve the no-overclaim rules.

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
- Phase 10 completed: metrics snapshots, relaxed-atomic counters, latency histogram, CLI metrics output, docs, and broker end-to-end benchmark.
- Phase 11 completed: GitHub Actions CI, sanitizer workflow, clang-tidy static analysis, benchmark smoke workflow, CMake sanitizer options, install/export package rules, contributing guide, changelog, and release checklist.
- Phase 12 completed: batch broker API, experimental zero-copy SPSC reservation API, spin-wait utilities, Linux-first CPU affinity helpers, Phase 12 benchmarks, low-latency tuning docs, and HFT-style design notes.
- Phase 13 completed: final portfolio README, architecture documentation, inline Mermaid diagrams, limitations doc, interview notes, benchmark reporting polish, release checklist updates, and v0.1.0 candidate release notes.
