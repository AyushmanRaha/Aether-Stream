# Benchmark Methodology

Aether-Stream includes benchmark executables for local experiments and regression checks. Benchmark numbers are not published unless raw outputs and environment metadata are committed and linked.

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

## Raw result directory

The runner writes output under:

```text
benchmark-results/YYYYMMDD-HHMMSS/
```

Preserve the generated `.txt`, `.json`, and environment metadata files before publishing any values.

## Build requirements

Publishable measurements should use Release builds. Debug builds, sanitizer builds, and benchmark smoke jobs are useful for validation but should not be reported as performance results.

## Smoke checks are not results

The benchmark-smoke workflow intentionally uses short runs to confirm executable health. Short smoke runs are not stable benchmark evidence and should not be copied into performance tables.

## Publishing future numbers honestly

Future results, including planned M1 MacBook Air local measurements, should include:

- environment summary;
- exact benchmark command;
- raw output path;
- copied values from raw output only;
- notes about thermal, scheduler, affinity, or platform limitations.

Do not add throughput, latency, percentile, CPU, OS, or memory claims unless committed raw output proves them.
