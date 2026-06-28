# Performance Results

## Current status

TODO: No official measured results have been committed yet. Run ./scripts/run_benchmarks.sh from a Release-capable local environment and paste only measured numbers with the raw output path.

## Latest run

TODO: Add run timestamp and git commit after an official measured run.

## Environment

TODO: Summarize the relevant entries from `benchmark-results/<timestamp>/environment.txt`.

## Commands

```sh
TODO: ./scripts/run_benchmarks.sh
```

## Raw output location

TODO: `benchmark-results/<timestamp>/`

## SPSC throughput results table

| Benchmark | Capacity | Messages | Throughput | Producer retries | Consumer retries | Raw file |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| TODO | TODO | TODO | TODO | TODO | TODO | TODO |

## SPSC latency results table

| Benchmark | Capacity | Messages | p50 ns | p95 ns | p99 ns | p999 ns | min ns | max ns | Raw file |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO |

## Payload-size comparison table

| Benchmark | Capacity | Payload bytes | Messages | Items/s | Bytes/s | Raw file |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| TODO | TODO | TODO | TODO | TODO | TODO | TODO |

## Broker end-to-end results table

| Benchmark | Mode | Capacity | Payload bytes | Messages/items | Items/s | WAL bytes written | WAL records written | Raw file |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO |

## Interpretation notes

TODO: Add interpretation only after measured raw outputs exist. Keep notes specific to the hardware, OS, build type, and benchmark command used for the run.

## Limitations

- These results are not currently official measured numbers.
- SPSC tables describe queue benchmarks.
- The broker end-to-end table is for the Phase 10 local broker benchmark once measured.
- Do not treat broker end-to-end results as networking, IPC, production persistence, or HFT claims.
- Debug builds must not be reported.
- Laptop, macOS, thermally constrained, or busy-system results should be labeled as development measurements.
- Do not add production-ready or HFT-ready claims.
