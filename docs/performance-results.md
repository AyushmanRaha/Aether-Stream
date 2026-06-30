# Performance Results

## Current status

No official benchmark run has been published yet. This repository currently contains no committed `benchmark-results/YYYYMMDD-HHMMSS/` raw outputs, so this document intentionally contains no fabricated throughput, latency, p99, p999, or hardware-specific numbers.

## How to publish a result

1. Run `./scripts/run_benchmarks.sh` from the repository root.
2. Copy environment metadata from `benchmark-results/YYYYMMDD-HHMMSS/environment.txt`.
3. Copy measured numbers from the raw `.txt` or `.json` benchmark outputs.
4. Link each table row to the exact raw output filename/path.

## Canonical commands

```sh
./scripts/run_benchmarks.sh
```

Short exploratory command:

```sh
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s
```

## Latest official run

Not yet published.

## Environment

Not yet published. Future entries must summarize `environment.txt` from the raw output directory.

## Raw output location

Not yet published. Future entries must link to `benchmark-results/YYYYMMDD-HHMMSS/`.

## SPSC throughput results table

| Benchmark | Capacity | Messages | Throughput | Producer retries | Consumer retries | Raw file |
|---|---:|---:|---:|---:|---:|---|
| Not yet published | - | - | - | - | - | - |

## SPSC latency results table

| Benchmark | Capacity | Messages | p50 ns | p95 ns | p99 ns | p999 ns | min ns | max ns | Raw file |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---|
| Not yet published | - | - | - | - | - | - | - | - | - |

## Payload-size comparison table

| Benchmark | Capacity | Payload bytes | Messages | Items/s | Bytes/s | Raw file |
|---|---:|---:|---:|---:|---:|---|
| Not yet published | - | - | - | - | - | - |

## Broker end-to-end results table

| Benchmark | Mode | Capacity | Payload bytes | Messages/items | Items/s | WAL bytes written | WAL records written | Raw file |
|---|---|---:|---:|---:|---:|---:|---:|---|
| Not yet published | - | - | - | - | - | - | - | - |

## Batch publish results table

| Benchmark | Capacity | Batch size | Payload bytes | Items/s | Raw file |
|---|---:|---:|---:|---:|---|
| Not yet published | - | - | - | - | - |

## Zero-copy SPSC results table

| Benchmark | Capacity | Payload bytes | Items/s | Raw file |
|---|---:|---:|---:|---|
| Not yet published | - | - | - | - |

## Spin-wait microbenchmark results table

| Benchmark | Operation | Time | Raw file |
|---|---|---:|---|
| Not yet published | - | - | - |

## Warnings

- No official numbers without raw outputs.
- Laptop/macOS results are development results unless clearly labeled otherwise.
- No HFT-ready claims.
- Spin-wait microbenchmark results are not broker latency.
- Debug builds must not be reported.
- Benchmark smoke checks are not official performance results.
