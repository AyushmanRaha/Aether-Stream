# Benchmark Methodology

## Scope

The benchmark suite covers:

- SPSC throughput;
- SPSC latency;
- payload-size comparison;
- broker end-to-end flow;
- batch publish;
- zero-copy SPSC;
- spin-wait microbenchmark.

Benchmarks are for reproducible local experiments and honest reporting. They are not networking, IPC, production durability, or HFT-readiness claims.

## Canonical benchmark workflow

Run from the repository root:

```sh
./scripts/run_benchmarks.sh
```

For shorter exploratory runs:

```sh
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s
```

The runner configures a Release build with tests, examples, tools, apps, and benchmarks enabled; builds benchmark targets; runs CTest; captures environment metadata; and writes raw benchmark output.

## Raw result storage

```text
benchmark-results/YYYYMMDD-HHMMSS/
```

Each benchmark emits `.txt` and `.json` files. `environment.txt` captures metadata.

## Captured environment metadata

The runner records available details including:

- git commit/status;
- OS details;
- CPU/RAM details where available;
- CMake version;
- compiler version.

## Benchmark executables

- `bench_spsc_throughput`
- `bench_spsc_latency`
- `bench_payload_sizes`
- `bench_broker_end_to_end`
- `bench_batch_publish`
- `bench_zero_copy_spsc`
- `bench_spin_wait`

## Reporting rules

Explicitly forbidden:

- Debug benchmark reporting;
- cherry-picked results without raw output paths;
- manual stress tool output as benchmark results;
- HFT or production claims from local laptop results;
- p50/p95/p99/p999 or throughput numbers that are not copied from raw benchmark outputs.

## Platform caveats

macOS and Apple Silicon are excellent development environments but may have power-management, scheduler, and thermal behavior that makes results unsuitable for final low-latency claims. Linux pinned-core measurements can be more controlled, but only if the environment, kernel behavior, CPU isolation, governor, background load, and raw outputs are documented. CPU affinity helpers are Linux-first; macOS fallback behavior means affinity conclusions must be platform-specific.

## Benchmark meanings

- SPSC throughput measures ordered queue transfer throughput.
- SPSC latency measures approximate queue transfer visibility for timestamped payloads.
- Payload-size comparison measures queue behavior with different object sizes.
- Broker end-to-end measures local publish/consume paths with WAL-disabled and WAL-enabled variants.
- Batch publish compares single-message and batch broker usage patterns.
- Zero-copy SPSC compares normal insertion against reserve/construct/commit insertion.
- Spin-wait is a synthetic primitive microbenchmark and is not broker latency.

## Publishing a result

1. Run `./scripts/run_benchmarks.sh`.
2. Keep the full `benchmark-results/YYYYMMDD-HHMMSS/` directory.
3. Copy environment details from `environment.txt`.
4. Copy measured values from raw `.txt` or `.json` files.
5. Link every published table row to its raw file.
