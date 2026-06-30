# Low-Latency Tuning Notes

These notes cover practical local tuning considerations for Aether-Stream's latency-oriented APIs. They do not contain official performance claims.

## Build type

Use Release builds for benchmark measurements. Debug builds are useful for correctness, but their timings should not be reported as performance results.

## Batching

`BatchBroker<T, Capacity>` batches local in-memory broker calls. It can reduce repeated call overhead in suitable workloads, but it does not change the SPSC contract and does not add persistence semantics.

## Zero-copy SPSC

`ZeroCopySpsc<T, Capacity>` lets the producer reserve a slot, construct in place, and commit or cancel. It remains experimental and should be compared against the regular SPSC queue with raw benchmark output.

## Spin waiting

`SpinWait` combines short active waiting with periodic yielding. Use it only when the application can afford CPU usage and when measurement conditions are documented.

## CPU affinity

CPU affinity support is platform-dependent. Linux uses `pthread_setaffinity_np`; unsupported platforms report unsupported behavior rather than pretending to pin threads.

## Benchmark guidance

Use `./scripts/run_benchmarks.sh` for publishable local numbers. The runner creates `benchmark-results/YYYYMMDD-HHMMSS/` as local scratch output; publishable evidence may be separate sanitized raw files or one consolidated redacted Markdown transcript under `docs/benchmark-results/`. Benchmark smoke jobs are not official results.

For the current published local run, see [Performance results](performance-results.md) and the detailed [M1 MacBook Air benchmark run](benchmark-results/m1-macbook-air-2026-06-29.md).
