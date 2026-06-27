# Benchmark Methodology

## Scope

Phase 5 adds a benchmark framework for measuring the existing `SpscRingBuffer<T, Capacity>` implementation. The benchmarks are intended to make local performance experiments reproducible and to keep reported numbers tied to raw output files.

## What is being benchmarked in Phase 5

- SPSC producer-to-consumer message throughput.
- Approximate per-message SPSC queue transfer latency using timestamped payloads.
- Throughput impact of payload object sizes of 8B, 32B, 64B, 256B, and 1024B.

## What is not being benchmarked yet

Phase 5 benchmarks measure SPSC queue behavior only. They do not measure a broker, WAL, mmap persistence, CLI workflow, metrics subsystem, network path, end-to-end durability, or any Phase 6+ feature.

## Build configuration

Performance reporting requires a Release build. Debug numbers must not be reported as performance results because compiler optimization, assertions, and debug-friendly code generation can dominate the measurement.

## Required command

Use the benchmark runner from the repository root:

```sh
./scripts/run_benchmarks.sh
```

To shorten exploratory runs, pass Google Benchmark flags that are appended to every benchmark executable:

```sh
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s
```

## Benchmark executables

Phase 5 builds these executable targets when `AETHER_BUILD_BENCHMARKS=ON`:

- `bench_spsc_throughput`
- `bench_spsc_latency`
- `bench_payload_sizes`

They are emitted under `${CMAKE_BINARY_DIR}/benchmarks`.

## Throughput benchmark definition

The throughput benchmark creates a fresh SPSC queue for each benchmark iteration, starts one producer thread and one consumer thread, transfers an ordered sequence of messages, validates sequence order, and reports items processed. Producer and consumer back off with `SpinWait` when the queue is full or empty.

## Latency benchmark definition

The latency benchmark transfers timestamped 64-byte payloads between one producer and one consumer. The producer stores a monotonic timestamp immediately before attempting to publish a message, and the consumer records `now - timestamp` after popping the message. Samples are preallocated before timing, and percentile calculation is performed after the measured transfer window.

The reported latency distribution is a development measurement of queue transfer and consumer visibility behavior. It is not an end-to-end broker latency measurement.

## Payload-size benchmark definition

The payload-size benchmark uses fixed-size payload objects of 8B, 32B, 64B, 256B, and 1024B. It transfers ordered payloads through the SPSC queue, validates sequence order, and reports items and bytes processed.

## Capacities used

- Throughput: 64, 256, 1024, and 65536.
- Latency: 1024 and 65536.
- Payload-size comparison: 1024 and 65536.

## Payload sizes used

Payload-size comparison covers 8B, 32B, 64B, 256B, and 1024B payload objects.

## Warmup/min-time policy

The runner uses Google Benchmark's default execution model with `--benchmark_min_time=2s` unless the caller supplies an override. Shorter runs are acceptable for smoke checks, but they should not be treated as official performance results.

## Raw result storage

The runner writes raw outputs under:

```text
benchmark-results/YYYYMMDD-HHMMSS/
```

Each benchmark emits both console text and JSON:

- `bench_spsc_throughput.txt` and `.json`
- `bench_spsc_latency.txt` and `.json`
- `bench_payload_sizes.txt` and `.json`

## Environment metadata captured

The runner stores `environment.txt` with available metadata including git commit and status, OS details, CPU/core details when available, CMake version, and C++ compiler version.

## How to reproduce

1. Start from a clean checkout or record local changes in `environment.txt`.
2. Run `./scripts/run_benchmarks.sh` from the repository root.
3. Keep the generated `benchmark-results/YYYYMMDD-HHMMSS/` directory with any reported numbers.
4. Copy only measured values from the raw text or JSON files into `docs/performance-results.md`.

## Limitations and honesty notes

- Release builds are required for performance reporting.
- Debug numbers must not be reported.
- macOS and laptop measurements are development numbers.
- Apple Silicon power management, thermals, background tasks, and scheduler behavior can affect results.
- Final HFT-style claims would require controlled Linux benchmarking with pinned cores and a quieter system.
- Phase 5 benchmarks measure SPSC queue behavior only, not broker, WAL, CLI, mmap persistence, or end-to-end durability.
- Do not invent numbers, omit raw output paths, or present manual stress-tool output as benchmark results.
