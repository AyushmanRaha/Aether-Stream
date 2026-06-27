# AGENTS.md

## Project summary

Aether-Stream is a planned C++20 ultra-low-latency lock-free asynchronous message broker library and eventual CLI toolkit. The current repository is a library foundation with an SPSC queue primitive; it is not yet a complete broker.

## Current phase

Phase 7 has been completed. The repository includes the Phase 0 setup, Phase 1 CMake/library/test skeleton, Phase 2 core public types and message model, Phase 3 SPSC ring buffer v1, Phase 4 SPSC concurrency-correctness hardening, Phase 5 benchmark framework and honest performance-reporting docs, Phase 6 memory-mapped file abstraction, and Phase 7 WAL writer/reader persistence foundation.

## Next phase

Phase 8 is next: broker integration. Do not add Phase 8 implementation unless the active task explicitly asks for that phase.

## Phase boundaries

- Phase 0 completed: documentation, setup, and configuration.
- Phase 1 completed: CMake project, library target, alias target, version API, smoke example, first CTest test, and local scripts.
- Phase 2 completed: core public types, message model, status/error handling, expected-like wrapper, and config structs.
- Phase 3 completed: header-only SPSC ring buffer v1, cache-line/platform helpers, basic SPSC example, and SPSC basic/wraparound tests.
- Phase 4 completed: SPSC hardening with move support, `size_approx`, acquire/release memory-order comments, queue tuning fields, utility helpers, concurrent/move-only/stress tests, and manual SPSC stress tool.
- Phase 5 completed: Google Benchmark wiring, SPSC throughput/latency/payload-size benchmarks, release benchmark runner, raw-result workflow, benchmark methodology doc, and performance-results template.
- Phase 6 completed: memory-mapped file abstraction, including `MmapFile`, POSIX mmap implementation, persistence tests, mmap smoke example, and mmap notes.
- Phase 7 completed: WAL record format, CRC32 checksum support, append-only WAL writer, sequential WAL reader, replay example, WAL tests, and format documentation.
- Phase 8+ later: broker behavior, CLI tools, metrics/diagnostics, CI, packaging, release work, and advanced tuning.

Do not add Phase 8+ implementation unless the active task explicitly asks for that phase.

## Current build targets

- `aether_stream`: main library target.
- `aether::stream`: alias target for consumers.
- `aether_smoke`: smoke example executable when examples are enabled.
- `aether_basic_spsc`: basic SPSC example executable when examples are enabled.
- `aether_mmap_smoke`: mmap smoke example executable when examples are enabled.
- `aether_wal_replay`: WAL replay example executable when examples are enabled.
- `aether_stress_spsc`: manual SPSC stress tool when tools are enabled.
- `aether_test_version`: version CTest executable when tests are enabled.
- `aether_test_status`: status CTest executable when tests are enabled.
- `aether_test_message`: message CTest executable when tests are enabled.
- `aether_test_spsc_basic`: basic SPSC CTest executable when tests are enabled.
- `aether_test_spsc_wraparound`: wraparound SPSC CTest executable when tests are enabled.
- `aether_test_spsc_concurrent`: concurrent ordered-transfer SPSC CTest executable when tests are enabled.
- `aether_test_spsc_move_only`: move-only payload SPSC CTest executable when tests are enabled.
- `aether_test_spsc_stress`: multi-capacity SPSC stress CTest executable when tests are enabled.
- `aether_test_mmap_file`: mmap file CTest executable when tests are enabled.
- `aether_test_wal_record`: WAL record-format CTest executable when tests are enabled.
- `aether_test_wal_writer`: WAL writer CTest executable when tests are enabled.
- `aether_test_wal_reader`: WAL reader CTest executable when tests are enabled.
- `aether_bench_spsc_throughput`: SPSC throughput benchmark when benchmarks are enabled.
- `aether_bench_spsc_latency`: SPSC latency benchmark when benchmarks are enabled.
- `aether_bench_payload_sizes`: SPSC payload-size benchmark when benchmarks are enabled.

## Current implemented components

- Public version API.
- Core type aliases and helpers.
- Status/error handling.
- Expected-like result wrapper.
- Queue, WAL, and broker config structs with validation helpers.
- Non-owning message header/view model.
- Header-only SPSC ring buffer for exactly one producer and exactly one consumer.
- Cache-line and platform detail helpers.
- Clock and thread utility helpers.
- RAII POSIX memory-mapped file wrapper (`MmapFile`).
- WAL record format.
- CRC32 checksum support.
- Append-only WAL writer.
- Sequential WAL reader.
- WAL replay example.
- Smoke, basic SPSC, and mmap smoke examples.
- Version, status, message, SPSC basic, SPSC wraparound, SPSC concurrent, SPSC move-only, SPSC stress, mmap file, and WAL tests.
- Manual SPSC stress-validation tool.
- Google Benchmark dependency wiring gated behind `AETHER_BUILD_BENCHMARKS`.
- SPSC throughput, per-message latency, and payload-size benchmark executables.
- Release-mode benchmark runner script with raw outputs under `benchmark-results/`.
- Benchmark methodology and performance-results template docs.

## Do not add unless requested by a phase

- Measured performance claims that are not backed by raw benchmark outputs.
- Broker implementation or broker APIs.
- CLI toolkit apps.
- Metrics or diagnostics systems.
- GitHub Actions CI, sanitizer job files, packaging, export/install logic, or release automation.
- External dependencies.

## Do not overclaim

Do not add fake performance numbers, MVP claims, production-ready claims, HFT-ready claims, or wording that implies broker-integrated durability, broker runtime recovery, CLI tooling, metrics, CI, packaging, or production persistence is complete. Standalone Phase 7 WAL writer/reader support exists, but it is not yet integrated into a broker and must not be described as a complete broker durability system. The manual SPSC stress tool is for correctness/stress validation only and must not be presented as a benchmark result.

## Style rules

- Use C++20 for C++ code.
- Use the repository `.clang-format` file for C++ formatting.
- Keep commits small and focused.
- Do not vendor dependencies unless a later phase explicitly asks for them.

## Expected future layout

Future phases may add or expand `apps/`, `tools/`, `cmake/`, `docs/`, `scripts/`, and `.github/workflows/` as needed. Do not create future-phase docs or directories early unless explicitly requested.

## Local verification

Useful local commands:

```sh
./scripts/run_tests.sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON -DAETHER_BUILD_EXAMPLES=ON -DAETHER_BUILD_TOOLS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
./build/debug/examples/smoke
./build/debug/examples/basic_spsc
./build/debug/examples/mmap_smoke
./build/debug/examples/wal_replay
./build/debug/tools/stress_spsc --messages 1000000 --capacity 1024
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s
```

## Cost/limit efficiency

- Read `AGENTS.md` and `docs/00-project-map.md` first before scanning the whole repository.
- Do not perform broad rewrites outside the requested phase.
- Do not fetch external dependencies unless asked.
- Keep diffs focused.
