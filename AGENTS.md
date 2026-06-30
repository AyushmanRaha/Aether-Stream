# AGENTS.md

## Project summary

Aether-Stream is a local C++20 systems project for studying and demonstrating bounded low-latency messaging primitives. It includes a reusable library, examples, CLI tools, tests, benchmarks, documentation, and CMake package export support. It is not production-ready, does not include networking, and does not publish official benchmark numbers without committed raw benchmark output.

## Current implemented components

- Public version API.
- Core type aliases and helpers.
- Status/error handling.
- Expected-like result wrapper.
- Queue, WAL, and broker configuration structs with validation helpers.
- Non-owning message header/view model.
- Header-only SPSC ring buffer for exactly one producer and exactly one consumer.
- Experimental zero-copy SPSC queue (`aether::ZeroCopySpsc<T, Capacity>`).
- Cache-line and platform detail helpers.
- Clock and thread utility helpers.
- `aether::utils::cpu_relax` and `aether::utils::SpinWait`.
- CPU affinity APIs with platform-specific support reporting.
- RAII POSIX-oriented memory-mapped file wrapper (`MmapFile`), with unsupported fallback on non-POSIX platforms.
- WAL record format, CRC32 checksum support, append-only WAL writer, and sequential WAL reader.
- In-memory broker API (`Broker<T, Capacity>`).
- Batch-oriented in-memory broker API (`BatchBroker<T, Capacity>`).
- WAL-backed persistent broker API (`PersistentBroker<T, Capacity>`).
- WAL-before-queue publish semantics.
- Typed replay for trivially copyable same-program payload types.
- Metrics snapshots, relaxed-atomic counters, and latency histogram.
- CLI argument parser and CLI apps: `aether_bench`, `aether_pub`, `aether_sub`, `aether_replay`, and `aether_inspect_wal`.
- Smoke, SPSC, mmap, WAL, and broker examples.
- CTest coverage for core types, queue behavior, mmap, WAL, broker APIs, metrics, CLI parsing, batch broker, and zero-copy SPSC.
- Benchmark executables for SPSC throughput, SPSC latency, payload sizes, broker end-to-end flow, batch publishing, zero-copy SPSC, and spin waiting.
- GitHub Actions CI, sanitizer workflow, benchmark smoke workflow, clang-tidy configuration, and install/export package rules.

## Current build targets

- `aether_stream`: main library target.
- `aether::stream`: public alias target for consumers.
- Examples when enabled: `aether_smoke`, `aether_basic_spsc`, `aether_mmap_smoke`, `aether_wal_replay`, `aether_broker_basic`, `aether_persistent_broker`.
- Tool when enabled: `aether_stress_spsc`.
- CLI apps when enabled: `aether_app_bench`, `aether_app_pub`, `aether_app_sub`, `aether_app_replay`, `aether_app_inspect_wal`.
- Tests when enabled: version, status, message, SPSC, mmap, WAL, broker, metrics, CLI args, batch broker, and zero-copy SPSC test executables.
- Benchmarks when enabled: `bench_spsc_throughput`, `bench_spsc_latency`, `bench_payload_sizes`, `bench_broker_end_to_end`, `bench_batch_publish`, `bench_zero_copy_spsc`, and `bench_spin_wait`.

## Important limitations

- Local library and CLI demos only; no networking, daemon, distributed broker, or live cross-process subscription semantics.
- SPSC only; do not imply MPSC or MPMC support.
- Fixed-size mmap-backed WAL; no rotation, repair, compaction, schema evolution, replication, or production recovery tooling.
- Typed replay is for trivially copyable same-program/same-platform payloads.
- CPU affinity support is platform-dependent and Linux-first.
- Native Windows behavior is not fully verified; prefer WSL2 guidance unless code and tests are updated and confirmed.

## No-overclaim rules

- Do not add fake performance numbers, production-readiness claims, unsupported latency guarantees, or wording that implies distributed or service behavior.
- Do not present stress tests, benchmark smoke jobs, sanitizers, or CI as proof of production readiness.
- Do not introduce new external dependencies unless explicitly requested and justified.
- Keep changes focused and codebase-grounded.

## Documentation style rules

- Use current-state descriptions instead of build-history language.
- Keep limitations visible near any feature that could be misunderstood.
- Use Mermaid diagrams directly in Markdown for architecture and flow diagrams; do not add generated diagram image assets unless explicitly requested.
- Update links whenever docs are renamed, deleted, or moved.
- Keep README and docs clear for both non-technical reviewers and technical readers.

## Benchmark honesty rules

- Official benchmark numbers are not published unless raw outputs and environment metadata are committed and linked.
- Use `./scripts/run_benchmarks.sh` for publishable local benchmark runs.
- Raw output directories use `benchmark-results/YYYYMMDD-HHMMSS/`.
- Do not commit generated benchmark results unless explicitly requested.
- Benchmark smoke checks are build/runtime checks only.

## Local verification commands

```sh
./scripts/format_all.sh --check

cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure

cmake -S . -B build/asan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_ENABLE_ASAN=ON \
  -DAETHER_ENABLE_UBSAN=ON
cmake --build build/asan
ctest --test-dir build/asan --output-on-failure

cmake -S . -B build/tsan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_ENABLE_TSAN=ON
cmake --build build/tsan
ctest --test-dir build/tsan --output-on-failure
```
