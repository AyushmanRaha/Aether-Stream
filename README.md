# Aether-Stream

Aether-Stream is a C++20 ultra-low-latency lock-free asynchronous message broker library under development. The repository now includes the reusable library foundation, message representation, SPSC queue primitive, mmap primitive, write-ahead log foundation, developer-facing in-memory broker API, WAL-backed persistent broker API, and Phase 9 terminal CLI demo toolkit.

## Current status

The repository is complete through Phase 9 of the phase-wise plan. It currently includes a real CMake build system, core public API types, status/error handling, a lightweight expected-like wrapper, configuration structs, a message model, an SPSC ring buffer, broker APIs, examples, CTest coverage, utility helpers, local scripts, a manual SPSC stress tool, Google Benchmark wiring, SPSC benchmark executables, benchmark reporting docs, a POSIX memory-mapped file abstraction, append-only WAL writer/reader support, typed replay for trivially copyable persistent broker events, and Phase 9 CLI demo apps.

It is not production-ready. Metrics/diagnostics, CI, packaging, and production-ready performance claims are not available yet. No official measured benchmark results have been committed yet.

## What exists today

### Build system

- Top-level CMake project using C++20.
- Library target: `aether_stream`.
- Public alias target for consumers: `aether::stream`.
- Options for enabling tests, examples, tools, CLI apps, warnings, warnings-as-errors, and benchmark builds.
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

### Examples

- `examples/smoke.cpp` links against `aether::stream` and prints the library version.
- `examples/basic_spsc.cpp` demonstrates integer queue usage and queueing `MessageHeader` values.
- `examples/broker_basic.cpp` demonstrates the in-memory broker API.
- `examples/persistent_broker.cpp` demonstrates WAL-backed publish, consume, flush, and typed replay.
- `examples/mmap_smoke.cpp` demonstrates a small mapped-file create/write/flush/close/reopen flow.
- `examples/wal_replay.cpp` demonstrates WAL append and sequential replay.

### CLI toolkit

- Phase 9 CLI apps build when `AETHER_BUILD_APPS=ON`.
- `aether_bench` runs a simple two-thread local broker demo benchmark without Google Benchmark.
- `aether_pub` writes generated `OrderEvent` records to a local WAL-backed persistent broker.
- `aether_sub` runs a local in-process subscriber demo or replays typed `OrderEvent` WAL records. It is not a network subscriber.
- `aether_replay` prints generic raw WAL record summaries and safe payload previews.
- `aether_inspect_wal` scans WAL files and prints format/count/offset summaries.
- Full usage is documented in `docs/cli-guide.md`.

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
- mmap file create/write/flush/close/reopen behavior, resize behavior, move ownership, and destructor-flush coverage;
- WAL record format, writer behavior, reader replay, partial-record handling, zero-filled tails, and corruption detection;
- CLI argument parsing, defaults, help flags, valid values, and invalid argument handling through `tests/test_cli_args.cpp` (`aether.cli.args`).

### Benchmarks

- Google Benchmark-based SPSC benchmarks build when `AETHER_BUILD_BENCHMARKS=ON`. These are separate from the Phase 9 `aether_bench` CLI demo benchmark, which is a terminal demonstration rather than part of the Google Benchmark suite.
- `bench_spsc_throughput` measures ordered producer/consumer throughput across queue capacities.
- `bench_spsc_latency` records approximate timestamped per-message transfer latency distributions.
- `bench_payload_sizes` compares throughput across 8B, 32B, 64B, 256B, and 1024B payload objects.
- `docs/benchmark-methodology.md` explains how results should be produced and interpreted.
- `docs/performance-results.md` is a template for measured results and currently contains no fabricated numbers.

### Scripts/tooling

- `scripts/run_tests.sh` configures, builds, and runs the local test suite.
- `scripts/format_all.sh` formats or checks C/C++ files in the repository source directories.
- `scripts/bootstrap_macos.sh` checks for common macOS development tools and creates local build directories.
- `scripts/run_benchmarks.sh` configures a Release benchmark build, runs CTest, and stores raw benchmark output under `benchmark-results/`.
- `tools/stress_spsc.cpp` provides a manual SPSC stress executable when tools are enabled.

## What does not exist yet

Planned future work that is not currently implemented includes:

- metrics and diagnostics;
- CI, sanitizers, and packaging;
- advanced low-latency upgrades;
- final docs and portfolio packaging.

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

You can also use the local test shortcut:

```sh
./scripts/run_tests.sh
```

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

Phase 9 is complete. Planned future work begins with Phase 10 metrics and diagnostics, followed by:

- Add metrics and diagnostics.
- Add CI, sanitizer checks, and packaging.
- Evaluate advanced low-latency upgrades.
- Prepare final docs and portfolio packaging.

## Local setup

For macOS development checks, run:

```sh
chmod +x scripts/bootstrap_macos.sh
./scripts/bootstrap_macos.sh
```

The script checks for local tools and creates lightweight build directories. It does not install dependencies automatically.

## Repository docs

- [Project map](docs/00-project-map.md)
- [Learning roadmap](docs/01-learning-roadmap.md)
- [Ring buffer design](docs/ring-buffer-design.md)
- [Memory ordering](docs/memory-ordering.md)
- [Broker API](docs/broker-api.md)
- [CLI guide](docs/cli-guide.md)
- [Benchmark methodology](docs/benchmark-methodology.md)
- [Performance results](docs/performance-results.md)
- [Memory-mapped file notes](docs/mmap-notes.md)
- [WAL format](docs/wal-format.md)

## License

MIT.
