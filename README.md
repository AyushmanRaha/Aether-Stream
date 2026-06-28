# Aether-Stream

[![CI](https://github.com/AyushmanRaha/Aether-Stream/actions/workflows/ci.yml/badge.svg)](https://github.com/AyushmanRaha/Aether-Stream/actions/workflows/ci.yml)
[![Sanitizers](https://github.com/AyushmanRaha/Aether-Stream/actions/workflows/sanitizer.yml/badge.svg)](https://github.com/AyushmanRaha/Aether-Stream/actions/workflows/sanitizer.yml)
[![Benchmark smoke](https://github.com/AyushmanRaha/Aether-Stream/actions/workflows/benchmark-smoke.yml/badge.svg)](https://github.com/AyushmanRaha/Aether-Stream/actions/workflows/benchmark-smoke.yml)

Aether-Stream is a C++20 ultra-low-latency lock-free asynchronous message broker library under development. The repository now includes the reusable library foundation, message representation, SPSC queue primitive, mmap primitive, write-ahead log foundation, developer-facing in-memory broker API, WAL-backed persistent broker API, Phase 9 terminal CLI demo toolkit, Phase 10 metrics/diagnostics, Phase 11 CI/quality/package verification, and Phase 12 advanced low-latency APIs: a batch-oriented in-memory broker API, an experimental zero-copy SPSC queue, spin-wait utilities, a Linux-first CPU affinity helper with macOS no-op fallback, low-latency benchmark executables, and low-latency/HFT design notes.

## Current status

The repository is complete through Phase 12 of the phase-wise plan. It currently includes a real CMake build system, core public API types, status/error handling, a lightweight expected-like wrapper, configuration structs, a message model, an SPSC ring buffer, broker APIs, examples, CTest coverage, utility helpers, local scripts, a manual SPSC stress tool, Google Benchmark wiring, SPSC benchmark executables, benchmark reporting docs, a POSIX memory-mapped file abstraction, append-only WAL writer/reader support, typed replay for trivially copyable persistent broker events, Phase 9 CLI demo apps, Phase 10 metrics/diagnostics, Phase 11 CI/quality/package verification, and Phase 12 advanced low-latency APIs: a batch-oriented in-memory broker API, an experimental zero-copy SPSC queue, spin-wait utilities, a Linux-first CPU affinity helper with macOS no-op fallback, low-latency benchmark executables, and low-latency/HFT design notes.

It is not production-ready. CI, sanitizer, static-analysis, benchmark-smoke, and CMake package-install verification are now configured, but production-ready performance claims are not available. No official measured benchmark results have been committed yet.

## What exists today

### Build system

- Top-level CMake project using C++20.
- Library target: `aether_stream`.
- Public alias target for consumers: `aether::stream`.
- Options for enabling tests, examples, tools, CLI apps, warnings, warnings-as-errors, benchmark builds, sanitizers, clang-tidy, and install/export package rules.
- Google Benchmark is wired only when `AETHER_BUILD_BENCHMARKS=ON`.

### Core API

- Public version API in `include/aether/version.hpp`, implemented in `src/version.cpp`.
- Core type aliases, constants, and a power-of-two helper in `include/aether/core/types.hpp`.
- Status codes, status names/messages, and the `Status` type in `include/aether/core/status.hpp` and `src/core/status.cpp`.
- Lightweight C++20 `Expected<T>` and `Expected<void>` wrapper in `include/aether/core/expected.hpp`.
- Queue, WAL, and broker configuration structs with validation helpers in `include/aether/core/config.hpp`.

### Message model

- `MessageHeader`, `MessageView`, and `PayloadView` in `include/aether/message.hpp`.
- Helpers for checking payload size, constructing a non-owning message view, and validating that a view's header matches its payload span.

### SPSC queue

- `SpscRingBuffer<T, Capacity>` in `include/aether/spsc_ring_buffer.hpp`.
- The queue design supports exactly one producer thread and exactly one consumer thread. Multiple producers or multiple consumers are not supported.
- Capacity is a compile-time power-of-two template argument with a minimum value of 2.
- Available operations include `try_push`, `try_emplace`, `try_pop`, `empty`, `full`, `capacity`, and approximate size reporting through `size_approx`.
- The implementation uses acquire/release atomics for producer/consumer publication and cache-line padding for head and tail counters.
- Move-only payload behavior is tested, including `std::unique_ptr<int>` and a custom move-only object.

### Broker API

- In-memory broker API in `include/aether/broker.hpp`.
- WAL-backed persistent broker API in `include/aether/persistent_broker.hpp`.
- `Broker<T, Capacity>` exposes `try_publish`, `try_emplace`, and `try_consume` over the SPSC queue.
- `PersistentBroker<T, Capacity>` uses WAL-before-queue publishing: a successful publish appends to the WAL before the value becomes visible to the consumer.
- Typed replay reconstructs trivially copyable event values from WAL records for same-program/same-platform replay.
- Broker API documentation is in `docs/broker-api.md`.


### Advanced low-latency APIs

- Batch-oriented in-memory broker API: `BatchBroker<T, Capacity>` in `include/aether/batch_broker.hpp`.
- Experimental zero-copy SPSC queue: `ZeroCopySpsc<T, Capacity>` in `include/aether/zero_copy_spsc.hpp`, with reservation, construction, commit, and cancellation semantics.
- Spin-wait helpers: `aether::utils::SpinWait` and `aether::utils::cpu_relax()` in `include/aether/utils/spin_wait.hpp`.
- CPU affinity helpers: `aether::utils::CpuAffinityInfo`, `cpu_affinity_info()`, `cpu_affinity_supported()`, `pin_current_thread_to_cpu()`, and `clear_current_thread_affinity()` in `include/aether/utils/cpu_affinity.hpp`.
- CPU affinity support is Linux-first. macOS and unsupported platforms use safe no-op fallback behavior for affinity operations.

### Memory-mapped files

- Public RAII mmap wrapper in `include/aether/io/mmap_file.hpp`, implemented in `src/io/mmap_file.cpp`.
- POSIX `mmap` resource handling is isolated behind `aether::io::MmapFile`.
- Supported operations include create, open existing, flush, resize, close, byte-span access, and move-only ownership transfer.
- Persistence behavior is covered by a standalone CTest test.
- `examples/mmap_smoke.cpp` demonstrates creating, flushing, closing, and reopening a mapped file.

### WAL persistence

- Stable Phase 7 record format in `include/aether/wal/record.hpp` with explicit 40-byte little-endian serialization.
- CRC32 checksum helpers in `include/aether/wal/checksum.hpp` and `src/wal/checksum.cpp`.
- Append-only WAL writer in `include/aether/wal/wal_writer.hpp` and `src/wal/wal_writer.cpp`.
- Sequential WAL reader and replay helper in `include/aether/wal/wal_reader.hpp` and `src/wal/wal_reader.cpp`.
- WAL format documentation in `docs/wal-format.md`.
- `examples/wal_replay.cpp` demonstrates writing three records and replaying them.
- WAL tests cover record serialization, checksum behavior, writer appends, reader replay, zero-filled tails, partial records, and checksum corruption detection.

### Utilities

- Cache-line padding utility in `include/aether/detail/cache_line.hpp`.
- Platform/compiler/architecture detection and force-inline macro in `include/aether/detail/platform.hpp`.
- Monotonic nanosecond clock and stopwatch helper in `include/aether/utils/clock.hpp`.
- Thread yield, CPU relax, spin-wait, and current-thread naming helpers in `include/aether/utils/thread_utils.hpp`.
- Phase 12 spin-wait helpers in `include/aether/utils/spin_wait.hpp`.
- Phase 12 CPU affinity helpers in `include/aether/utils/cpu_affinity.hpp`.

### Examples

- `examples/smoke.cpp` links against `aether::stream` and prints the library version.
- `examples/basic_spsc.cpp` demonstrates integer queue usage and queueing `MessageHeader` values.
- `examples/broker_basic.cpp` demonstrates the in-memory broker API.
- `examples/persistent_broker.cpp` demonstrates WAL-backed publish, consume, flush, and typed replay.
- `examples/mmap_smoke.cpp` demonstrates a small mapped-file create/write/flush/close/reopen flow.
- `examples/wal_replay.cpp` demonstrates WAL append and sequential replay.

### CLI toolkit

- Phase 9 CLI apps build when `AETHER_BUILD_APPS=ON` and now print Phase 10 metrics summaries.
- `aether_bench` runs a simple two-thread local broker demo benchmark without Google Benchmark.
- `aether_pub` writes generated `OrderEvent` records to a local WAL-backed persistent broker.
- `aether_sub` runs a local in-process subscriber demo or replays typed `OrderEvent` WAL records. It is not a network subscriber.
- `aether_replay` prints generic raw WAL record summaries and safe payload previews.
- `aether_inspect_wal` scans WAL files and prints format/count/offset summaries.
- Full usage is documented in `docs/cli-guide.md`; metrics are documented in `docs/metrics.md`.

### Tests

CTest registers standalone test executables for:

- version constants and `version_string()`;
- status/error handling and expected-like results;
- message header/view construction and validation;
- basic SPSC behavior, full/empty behavior, FIFO order, and `MessageHeader` transfer;
- SPSC wraparound and rolling push/pop behavior;
- concurrent SPSC transfer of ordered sequence numbers between one producer and one consumer;
- move-only payload support, including move-only object behavior;
- stress coverage that checks multiple queue capacities and preserves order;
- in-memory broker publish/consume, full/empty handling, order, emplacement, move-only support, and runtime config validation;
- persistent broker open/config validation, WAL-before-queue behavior, WAL record readability, full-queue no-append behavior, and typed replay;
- metrics counters, snapshots, reset behavior, WAL/recovery counter increments, and `tests/test_counters.cpp` registered as `aether.metrics.counters`;
- latency histogram empty/single/multiple-sample statistics, nearest-rank percentiles, clamping, clear, reserve behavior, and `tests/test_latency_histogram.cpp` registered as `aether.metrics.latency_histogram`;
- mmap file create/write/flush/close/reopen behavior, resize behavior, move ownership, and destructor-flush coverage;
- WAL record format, writer behavior, reader replay, partial-record handling, zero-filled tails, and corruption detection;
- CLI argument parsing, defaults, help flags, valid values, and invalid argument handling through `tests/test_cli_args.cpp` (`aether.cli.args`).
- batch broker batch ordering, partial batch behavior, empty/full edge cases, and invalid config behavior through `tests/test_batch_broker.cpp` (`aether.batch_broker`).
- zero-copy SPSC reservation, commit, cancel, destructor cancellation, active reservation guard, FIFO, full behavior, wraparound, and move-only payload support through `tests/test_zero_copy_spsc.cpp` (`aether.zero_copy_spsc`).

### Benchmarks

- Google Benchmark-based SPSC and broker end-to-end benchmarks build when `AETHER_BUILD_BENCHMARKS=ON`. These are separate from the Phase 10 `aether_bench` CLI demo benchmark, which is a terminal demonstration rather than part of the Google Benchmark suite.
- `bench_spsc_throughput` measures ordered producer/consumer throughput across queue capacities.
- `bench_spsc_latency` records approximate timestamped per-message transfer latency distributions.
- `bench_payload_sizes` compares throughput across 8B, 32B, 64B, 256B, and 1024B payload objects.
- `bench_broker_end_to_end` measures a local publish-to-consume path with WAL disabled and enabled, without making official performance claims.
- `bench_batch_publish` compares single-message broker publish/consume patterns with batch broker publish/consume patterns.
- `bench_zero_copy_spsc` compares normal SPSC insertion with zero-copy reserve/construct/commit insertion.
- `bench_spin_wait` compares `cpu_relax`, `std::this_thread::yield`, and `SpinWait`/backoff overhead.
- Phase 12 benchmarks are comparison, smoke, and development benchmarks only; they are not official HFT claims.
- `docs/benchmark-methodology.md` explains how results should be produced and interpreted.
- `docs/performance-results.md` is a template for measured results and currently contains no fabricated numbers.
- `docs/low-latency-tuning.md` documents Phase 12 batching, zero-copy, spin-wait, CPU-affinity, cache-line, and benchmark-honesty notes.
- `docs/hft-design-notes.md` documents Phase 12 HFT-style design tradeoffs and explicit limitations.

### Quality automation

- GitHub Actions workflows run format checks, Ubuntu/macOS Debug and Release builds, CTest, clang-tidy, sanitizer builds, benchmark smoke checks, and package install smoke checks.
- Sanitizer flags are centralized in `cmake/AetherSanitizers.cmake` and are enabled only through explicit CMake options.
- Install/export package rules are centralized in `cmake/AetherInstall.cmake` and export the consumer target as `aether::stream`.
- A moderate `.clang-tidy` configuration is available for opt-in local and CI static analysis.

### Scripts/tooling

- `scripts/run_tests.sh` configures, builds, and runs the local test suite.
- `scripts/format_all.sh` formats or checks C/C++ files in the repository source directories.
- `scripts/bootstrap_macos.sh` checks for common macOS development tools and creates local build directories.
- `scripts/run_benchmarks.sh` configures a Release benchmark build, runs CTest, and stores raw benchmark output under `benchmark-results/`; it runs all seven benchmark executables, including the Phase 12 benchmark executables.
- `tools/stress_spsc.cpp` provides a manual SPSC stress executable when tools are enabled.

## What does not exist yet

Planned future work that is not currently implemented includes:

- final Phase 13 documentation and portfolio packaging;
- official measured benchmark results from controlled runs;
- networking or a live inter-process broker service;
- MPSC/MPMC queues;
- production-ready or HFT-ready guarantees.


## Metrics and diagnostics

Phase 10 adds lightweight observability without changing broker semantics:

- always-on relaxed-atomic broker counters;
- `metrics_snapshot()`, `snapshot()`, and `reset_metrics()` on broker APIs;
- diagnostic `LatencyHistogram` for tests, tools, and benchmarks;
- an end-to-end broker benchmark covering in-memory and WAL-backed paths;
- concise metrics summaries in CLI output.

See [`docs/metrics.md`](docs/metrics.md) for field semantics, WAL/recovery metrics, and histogram percentile behavior.

## Build and test

Configure, build, and run the registered CTest suite with tests, examples, and tools enabled:

```sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON -DAETHER_BUILD_EXAMPLES=ON -DAETHER_BUILD_TOOLS=ON -DAETHER_BUILD_APPS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
```

Run the smoke example:

```sh
./build/debug/examples/smoke
```

Expected output:

```text
Aether-Stream 0.1.0
```

Run the basic SPSC example:

```sh
./build/debug/examples/basic_spsc
```

Run the broker examples:

```sh
./build/debug/examples/broker_basic
./build/debug/examples/persistent_broker
```

Run the mmap smoke example:

```sh
./build/debug/examples/mmap_smoke
```

Run the WAL replay example:

```sh
./build/debug/examples/wal_replay
```

Run only broker-related CTest coverage:

```sh
ctest --test-dir build/debug --output-on-failure -R broker
```

Run only WAL-related CTest coverage:

```sh
ctest --test-dir build/debug --output-on-failure -R wal
```

Run only mmap-related CTest coverage:

```sh
ctest --test-dir build/debug --output-on-failure -R mmap
```

Run only Phase 12 low-latency CTest coverage:

```sh
ctest --test-dir build/debug --output-on-failure -R "batch_broker|zero_copy"
```

You can also use the local test shortcut:

```sh
./scripts/run_tests.sh
```

## Quality checks

Check formatting locally with:

```sh
./scripts/format_all.sh --check
```

Configure an ASAN/UBSAN build with:

```sh
cmake -S . -B build/asan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_ENABLE_ASAN=ON \
  -DAETHER_ENABLE_UBSAN=ON
```

Configure a TSAN build separately with:

```sh
cmake -S . -B build/tsan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_ENABLE_TSAN=ON
```

For package install smoke checks, configure with `-DAETHER_ENABLE_INSTALL=ON`, run `cmake --install`, then verify a temporary consumer project can call `find_package(AetherStream CONFIG REQUIRED)` and link `aether::stream`. See [Contributing](CONTRIBUTING.md) and the [release checklist](docs/release-checklist.md) for the full command sequence.

## CLI demo apps

Build with `-DAETHER_BUILD_APPS=ON`, then inspect help for each app:

```sh
./build/release/apps/aether_bench --help
./build/release/apps/aether_pub --help
./build/release/apps/aether_sub --help
./build/release/apps/aether_replay --help
./build/release/apps/aether_inspect_wal --help
```

Quick demo flow:

```sh
./build/release/apps/aether_bench --messages 100000 --payload-size 64 --capacity 1024
./build/release/apps/aether_pub --wal data/sample.wal --messages 1000
./build/release/apps/aether_inspect_wal --wal data/sample.wal
./build/release/apps/aether_replay --wal data/sample.wal --limit 10
./build/release/apps/aether_sub --wal data/sample.wal --limit 10
```

These are local terminal demos only. They do not add networking, a live inter-process broker, production persistence, or official benchmark claims.

## Benchmarks

Build benchmark targets manually with:

```sh
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release -DAETHER_BUILD_TESTS=ON -DAETHER_BUILD_BENCHMARKS=ON
cmake --build build/release
./build/release/benchmarks/bench_batch_publish --benchmark_min_time=0.1s
./build/release/benchmarks/bench_zero_copy_spsc --benchmark_min_time=0.1s
./build/release/benchmarks/bench_spin_wait --benchmark_min_time=0.1s
```

Run the reproducible benchmark workflow, including a Release configure/build and CTest pre-check, with:

```sh
./scripts/run_benchmarks.sh
```

Raw benchmark outputs are written under `benchmark-results/`. See [Benchmark methodology](docs/benchmark-methodology.md) and [Performance results](docs/performance-results.md). No performance numbers should be reported unless they come from a measured run with raw outputs.

## Manual stress tool

The manual SPSC stress tool is built when `AETHER_BUILD_TOOLS=ON` is passed to CMake:

```sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TOOLS=ON
cmake --build build/debug --target aether_stress_spsc
./build/debug/tools/stress_spsc --messages 1000000 --capacity 1024
```

Supported capacities are `64`, `256`, `1024`, `4096`, and `65536`. The tool transfers ordered integer values between one producer and one consumer, then reports elapsed time, retry counts, and validation status for that run. Its output is for manual stress validation only and is not a benchmark result.

## Formatting

Format C++ source files in known source directories with:

```sh
./scripts/format_all.sh
```

Check formatting without modifying files with:

```sh
./scripts/format_all.sh --check
```

## Development roadmap

Phase 12 is complete. Planned future work is Phase 13 final documentation, portfolio packaging, diagrams, release notes, and final presentation polish. Do not treat the existing Phase 12 APIs or benchmarks as production-ready or HFT-ready guarantees.

## Local setup

For macOS development checks, run:

```sh
chmod +x scripts/bootstrap_macos.sh
./scripts/bootstrap_macos.sh
```

The script checks for local tools and creates lightweight build directories. It does not install dependencies automatically.

## Repository docs

- [Contributing guide](CONTRIBUTING.md)
- [Changelog](CHANGELOG.md)
- [Release checklist](docs/release-checklist.md)
- [Project map](docs/00-project-map.md)
- [Learning roadmap](docs/01-learning-roadmap.md)
- [Ring buffer design](docs/ring-buffer-design.md)
- [Memory ordering](docs/memory-ordering.md)
- [Broker API](docs/broker-api.md)
- [CLI guide](docs/cli-guide.md)
- [Metrics and diagnostics](docs/metrics.md)
- [Benchmark methodology](docs/benchmark-methodology.md)
- [Performance results](docs/performance-results.md)
- [Low-latency tuning](docs/low-latency-tuning.md)
- [HFT-style design notes](docs/hft-design-notes.md)
- [Memory-mapped file notes](docs/mmap-notes.md)
- [WAL format](docs/wal-format.md)

## License

MIT.
