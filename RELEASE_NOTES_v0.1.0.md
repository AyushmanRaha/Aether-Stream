# Aether-Stream v0.1.0 Release Notes

## Release status

Documentation-ready v0.1.0 draft. Do not treat this file as evidence that a GitHub release or git tag exists unless `v0.1.0` has been created in the repository.

## Highlights

- C++20 library target with public alias `aether::stream`.
- Lock-free SPSC ring buffer for exactly one producer and one consumer.
- In-memory broker, batch broker, and WAL-backed persistent broker APIs.
- mmap-backed WAL persistence with explicit record format and CRC32 validation.
- CLI toolkit for local demos, WAL publishing, replay, and inspection.
- Metrics counters, snapshots, and latency histogram.
- Benchmark suite and raw-output benchmark runner.
- CI, sanitizers, clang-tidy, format checks, benchmark smoke, and package install verification.
- Experimental zero-copy SPSC, spin-wait helpers, CPU-affinity helper, low-latency benchmarks, and tuning documentation.

## What is included

- Public headers under `include/aether/`.
- Library implementation under `src/`.
- Examples under `examples/`.
- CLI applications under `apps/`.
- CTest test executables under `tests/`.
- Benchmark executables under `benchmarks/`.
- Documentation under `docs/`.
- CMake install/export package support.

## What is intentionally not included

- Networking or IPC broker service behavior.
- Live cross-process subscriptions.
- MPSC or MPMC queues.
- Production durability, recovery, or WAL repair guarantees.
- Auth, TLS, ACLs, replication, clustering, or service discovery.
- Official benchmark numbers without raw `./scripts/run_benchmarks.sh` output.

## Build and test commands

```sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug   -DAETHER_BUILD_TESTS=ON   -DAETHER_BUILD_EXAMPLES=ON   -DAETHER_BUILD_TOOLS=ON   -DAETHER_BUILD_APPS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
```

## Benchmark workflow

```sh
./scripts/run_benchmarks.sh
```

For shorter exploratory runs:

```sh
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s
```

Raw outputs are written to `benchmark-results/YYYYMMDD-HHMMSS/` and must be preserved before publishing any performance table.

## Known limitations

See `docs/limitations.md`. In short: SPSC only, local-only, fixed-size mmap WAL, no WAL rotation, no production recovery tooling, typed replay is same-program/same-platform for trivially copyable payloads, and benchmark numbers must be Release-build raw-output-backed measurements.

## Suggested tag command

Do not run this automatically. Use it only after release verification is complete:

```sh
git tag -a v0.1.0 -m "Aether-Stream v0.1.0"
git push origin v0.1.0
```
