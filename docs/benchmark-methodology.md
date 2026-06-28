# Benchmark Methodology

## Scope

The benchmark suite contains the Phase 5 SPSC queue benchmarks, the Phase 10 broker end-to-end benchmark, and the Phase 12 low-latency comparison benchmarks. The benchmarks are intended to make local performance experiments reproducible and to keep reported numbers tied to raw output files.

## Phase 5 SPSC benchmarks

- SPSC producer-to-consumer message throughput.
- Approximate per-message SPSC queue transfer latency using timestamped payloads.
- Throughput impact of payload object sizes of 8B, 32B, 64B, 256B, and 1024B.

## Phase 10 broker end-to-end benchmark

`bench_broker_end_to_end` measures local publish-to-consume paths. It includes WAL-disabled in-memory broker variants and WAL-enabled persistent broker variants. It reports counters such as published, consumed, WAL bytes written, and WAL records written.

This benchmark is still local and in-process. It does not measure networking, IPC, production durability, fsync guarantees, or full system latency. It does not create official performance claims by itself.

## Phase 12 low-latency benchmarks

- `bench_batch_publish` compares single-message broker publish/consume patterns against batch broker publish/consume patterns.
- `bench_zero_copy_spsc` compares the normal SPSC `try_emplace`/push path against the zero-copy reserve/construct/commit path.
- `bench_spin_wait` compares `cpu_relax`, `std::this_thread::yield`, and `SpinWait`/backoff overhead.

These are comparison, smoke, and development benchmarks. They do not prove end-to-end trading-system latency or HFT readiness.

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

These executable targets build when `AETHER_BUILD_BENCHMARKS=ON`:

- `bench_spsc_throughput`
- `bench_spsc_latency`
- `bench_payload_sizes`
- `bench_broker_end_to_end`
- `bench_batch_publish`
- `bench_zero_copy_spsc`
- `bench_spin_wait`

They are emitted under `${CMAKE_BINARY_DIR}/benchmarks`.

## Throughput benchmark definition

The throughput benchmark creates a fresh SPSC queue for each benchmark iteration, starts one producer thread and one consumer thread, transfers an ordered sequence of messages, validates sequence order, and reports items processed. Producer and consumer back off with `SpinWait` when the queue is full or empty.

## Latency benchmark definition

The latency benchmark transfers timestamped 64-byte payloads between one producer and one consumer. The producer stores a monotonic timestamp immediately before attempting to publish a message, and the consumer records `now - timestamp` after popping the message. Samples are preallocated before timing, and percentile calculation is performed after the measured transfer window.

The reported latency distribution is a development measurement of queue transfer and consumer visibility behavior. It is not an end-to-end broker latency measurement.

## Payload-size benchmark definition

The payload-size benchmark uses fixed-size payload objects of 8B, 32B, 64B, 256B, and 1024B. It transfers ordered payloads through the SPSC queue, validates sequence order, and reports items and bytes processed.

## Batch benchmark definition

The batch benchmark compares API overhead and throughput patterns between single-message broker operations and batch publish/consume operations. It is not an end-to-end trading-system latency measurement.

## Zero-copy benchmark definition

The zero-copy benchmark compares direct in-slot construction through reserve/construct/commit against the normal queue insertion path. It is intended to evaluate local API overhead and queue behavior, not full application latency.

## Spin-wait benchmark definition

The spin-wait benchmark is a synthetic microbenchmark of waiting primitives. It compares tight CPU relax hints, scheduler yielding, and `SpinWait` backoff behavior, but it is not an end-to-end queue or broker benchmark.

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
- `bench_broker_end_to_end.txt` and `.json`
- `bench_batch_publish.txt` and `.json`
- `bench_zero_copy_spsc.txt` and `.json`
- `bench_spin_wait.txt` and `.json`

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
- SPSC benchmarks measure queue behavior.
- `bench_broker_end_to_end` measures a local broker path with WAL off/on.
- None of these benchmarks are production, networking, IPC, or HFT claims.
- Phase 12 microbenchmarks are comparison tools only.
- Spin-wait benchmark results are especially hardware- and scheduler-sensitive.
- CPU affinity behavior differs by platform; macOS no-op fallback means affinity-related conclusions must be Linux-specific.
- Do not invent numbers, omit raw output paths, or present manual stress-tool output as benchmark results.
