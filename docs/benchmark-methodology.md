# Benchmark Methodology

Aether-Stream includes benchmark executables for local experiments and regression checks. Benchmark numbers are published only when measured output, environment metadata, command provenance, and caveats are committed and linked. Current published results are summarized in [Performance results](performance-results.md), with detailed consolidated evidence in [M1 MacBook Air benchmark run — 2026-06-29](benchmark-results/m1-macbook-air-2026-06-29.md).

## Scope

Benchmarks cover:

- SPSC throughput;
- SPSC latency;
- payload-size effects;
- broker end-to-end local publish/consume flow;
- batch publishing;
- zero-copy SPSC;
- spin-wait primitives.

They do not measure networking, distributed behavior, production durability, or cross-process service behavior.

## Canonical command

Run the suite from the repository root:

```sh
./scripts/run_benchmarks.sh
```

For shorter exploratory runs:

```sh
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s
```

## Result directories and publishable evidence

The runner writes local scratch output under:

```text
benchmark-results/YYYYMMDD-HHMMSS/
```

The top-level `benchmark-results/` directory remains local output and should not be force-added to the repository.

Published benchmark evidence may be committed either as separate sanitized raw text/JSON outputs or as a consolidated sanitized markdown transcript under `docs/benchmark-results/`. A consolidated transcript must preserve the measured rows, counters, environment summary, command, commit, platform warnings, and redaction policy.

A consolidated markdown file is acceptable only when it copies benchmark values directly from the generated outputs and does not invent values, omit important counters, or round away fields that are needed to understand the run. Sensitive local-only fields such as hostnames, absolute paths, and untracked local status may be redacted when the redaction policy is documented in the transcript.

## Build requirements

Publishable measurements should use Release builds. Debug builds, sanitizer builds, and benchmark smoke jobs are useful for validation but should not be reported as performance results.

## Benchmark executable names

The current benchmark executable names are:

- `bench_spsc_throughput`
- `bench_spsc_latency`
- `bench_payload_sizes`
- `bench_broker_end_to_end`
- `bench_batch_publish`
- `bench_zero_copy_spsc`
- `bench_spin_wait`

## Smoke checks are not results

The benchmark-smoke workflow intentionally uses short runs to confirm executable health. Short smoke runs are not stable benchmark evidence and should not be copied into performance tables.

## Publishing numbers honestly

Published results should include:

- environment summary;
- exact benchmark command;
- commit measured;
- raw output directory or consolidated evidence path;
- copied values from measured output only;
- all benchmark rows being summarized;
- relevant counters;
- notes about thermal, scheduler, affinity, or platform limitations;
- links from [Performance results](performance-results.md) to the detailed evidence.

Do not add throughput, latency, percentile, CPU, OS, or memory claims unless committed evidence proves them. Do not infer production latency from local synthetic measurements.
