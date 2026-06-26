# Aether-Stream

Aether-Stream is a C++20 ultra-low-latency lock-free asynchronous message broker library under development. The repository currently focuses on the reusable library foundation, message representation, and a single-producer/single-consumer queue primitive that later broker components can build on.

## Current status

The repository is complete through Phase 4 of the phase-wise plan. It currently includes a real CMake build system, core public API types, status/error handling, a lightweight expected-like wrapper, configuration structs, a message model, an SPSC ring buffer, examples, CTest coverage, utility helpers, local scripts, and a manual SPSC stress tool.

It is not yet a complete broker. Persistence, a broker API, benchmark framework and measured results, CLI toolkit, metrics/diagnostics, CI, packaging, and production-ready performance claims are not available yet.

## What exists today

### Build system

- Top-level CMake project using C++20.
- Library target: `aether_stream`.
- Public alias target for consumers: `aether::stream`.
- Options for enabling tests, examples, tools, warnings, warnings-as-errors, and benchmark builds.
- Benchmark support is explicitly not implemented by the current CMake configuration.

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

### Utilities

- Cache-line padding utility in `include/aether/detail/cache_line.hpp`.
- Platform/compiler/architecture detection and force-inline macro in `include/aether/detail/platform.hpp`.
- Monotonic nanosecond clock and stopwatch helper in `include/aether/utils/clock.hpp`.
- Thread yield, CPU relax, spin-wait, and current-thread naming helpers in `include/aether/utils/thread_utils.hpp`.

### Examples

- `examples/smoke.cpp` links against `aether::stream` and prints the library version.
- `examples/basic_spsc.cpp` demonstrates integer queue usage and queueing `MessageHeader` values.

### Tests

CTest registers standalone test executables for:

- version constants and `version_string()`;
- status/error handling and expected-like results;
- message header/view construction and validation;
- basic SPSC behavior, full/empty behavior, FIFO order, and `MessageHeader` transfer;
- SPSC wraparound and rolling push/pop behavior;
- concurrent SPSC transfer of ordered sequence numbers between one producer and one consumer;
- move-only payload support, including move-only object behavior;
- stress coverage that checks multiple queue capacities and preserves order.

### Scripts/tooling

- `scripts/run_tests.sh` configures, builds, and runs the local test suite.
- `scripts/format_all.sh` formats or checks C/C++ files in the repository source directories.
- `scripts/bootstrap_macos.sh` checks for common macOS development tools and creates local build directories.
- `tools/stress_spsc.cpp` provides a manual SPSC stress executable when tools are enabled.

## What does not exist yet

Planned work that is not currently implemented includes:

- broker API;
- mmap-backed file layer;
- WAL writer, reader, and recovery behavior;
- benchmark framework and measured results;
- CLI toolkit;
- metrics and diagnostics;
- CI, sanitizers, and packaging.

## Build and test

Configure, build, and run the registered CTest suite with tests, examples, and tools enabled:

```sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON -DAETHER_BUILD_EXAMPLES=ON -DAETHER_BUILD_TOOLS=ON
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

You can also use the local test shortcut:

```sh
./scripts/run_tests.sh
```

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

Phase 5 is next. Planned future work includes:

- Add benchmark framework and honest measured results.
- Add mmap-backed file abstraction.
- Add WAL record format, writer, reader, and recovery behavior.
- Add broker API over the queue and persistence layer.
- Add CLI tools for publishing, replaying, inspecting WAL files, and running demos.
- Add metrics, diagnostics, CI, sanitizers, and packaging.

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

## License

MIT.
