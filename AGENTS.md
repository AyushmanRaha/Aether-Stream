# AGENTS.md

## Project summary

Aether-Stream is a C++20 ultra-low-latency lock-free asynchronous message broker library under development. The current repository is complete through Phase 13: it includes the reusable foundation, SPSC queue, mmap layer, WAL writer/reader, in-memory broker API, WAL-backed persistent broker API, Phase 9 CLI toolkit, Phase 10 metrics/diagnostics/observability, Phase 11 CI/sanitizer/static-analysis/package verification, Phase 12 BatchBroker, ZeroCopySpsc, SpinWait/cpu_relax utility, CPU affinity helper, benchmarks, docs, and Phase 13 portfolio documentation, inline Mermaid diagrams, limitations, interview notes, and v0.1.0 release notes. It is not production-ready and does not include networking or official measured benchmark claims.

## Current phase

Phase 13 has been completed. The repository includes the Phase 0 setup, Phase 1 CMake/library/test skeleton, Phase 2 core public types and message model, Phase 3 SPSC ring buffer v1, Phase 4 SPSC concurrency-correctness hardening, Phase 5 benchmark framework and honest performance-reporting docs, Phase 6 memory-mapped file abstraction, Phase 7 WAL writer/reader persistence foundation, Phase 8 broker integration with in-memory broker API, persistent broker API, WAL-before-queue semantics, typed replay, broker examples, broker tests, and broker API docs, plus Phase 9 CLI argument parsing helpers, five CLI apps, CLI args tests, CLI guide, README demo flow, Phase 10 metrics snapshots, relaxed-atomic counters, latency histogram, CLI metrics output, metrics docs, broker end-to-end benchmark, Phase 11 GitHub Actions CI, sanitizer workflow, benchmark smoke workflow, CMake sanitizer options, clang-tidy integration, install/export package support, contributing guide, changelog, release checklist, Phase 12 batch broker API, experimental zero-copy SPSC, SpinWait/cpu_relax utilities, CPU affinity helpers, Phase 12 tests, Phase 12 benchmarks, low-latency tuning docs, HFT-style design notes, and Phase 13 final documentation/portfolio packaging.

## Next phase

Phase 13 has been implemented: final documentation, portfolio packaging, inline Mermaid diagrams, release notes, limitations, and interview-readiness polish are present. Future tasks should remain scoped and must not overclaim production readiness.

## Phase boundaries

- Phase 0 completed: documentation, setup, and configuration.
- Phase 1 completed: CMake project, library target, alias target, version API, smoke example, first CTest test, and local scripts.
- Phase 2 completed: core public types, message model, status/error handling, expected-like wrapper, and config structs.
- Phase 3 completed: header-only SPSC ring buffer v1, cache-line/platform helpers, basic SPSC example, and SPSC basic/wraparound tests.
- Phase 4 completed: SPSC hardening with move support, `size_approx`, acquire/release memory-order comments, queue tuning fields, utility helpers, concurrent/move-only/stress tests, and manual SPSC stress tool.
- Phase 5 completed: Google Benchmark wiring, SPSC throughput/latency/payload-size benchmarks, release benchmark runner, raw-result workflow, benchmark methodology doc, and performance-results template.
- Phase 6 completed: memory-mapped file abstraction, including `MmapFile`, POSIX mmap implementation, persistence tests, mmap smoke example, and mmap notes.
- Phase 7 completed: WAL record format, CRC32 checksum support, append-only WAL writer, sequential WAL reader, replay example, WAL tests, and format documentation.
- Phase 8 completed: in-memory broker API, WAL-backed persistent broker API, WAL-before-queue durability semantics, typed replay for trivially copyable event types, broker examples, broker tests, and broker API documentation.
- Phase 9 completed: CLI argument parsing helpers, `aether_bench`, `aether_pub`, `aether_sub`, `aether_replay`, `aether_inspect_wal`, CLI args tests, CLI guide, and README demo flow.
- Phase 10 completed: metrics snapshots, relaxed-atomic counters, latency histogram, CLI metrics output, docs, and broker end-to-end benchmark.
- Phase 11 completed: GitHub Actions CI, sanitizer jobs, clang-tidy static analysis, benchmark smoke workflow, CMake sanitizer configuration, CMake install/export package support, contributor guide, changelog, and release checklist.
- Phase 12 completed: batch broker API, experimental zero-copy SPSC reservation API, spin-wait utilities, Linux-first CPU affinity helpers, Phase 12 benchmarks, low-latency tuning docs, and HFT-style design notes.
- Phase 13 completed: final portfolio README, architecture docs, inline Mermaid diagrams, limitations doc, interview notes, benchmark reporting polish, release checklist updates, and v0.1.0 candidate release notes.

## Current build targets

- `aether_stream`: main library target.
- `aether::stream`: alias target for consumers.
- `aether_smoke`: smoke example executable when examples are enabled.
- `aether_basic_spsc`: basic SPSC example executable when examples are enabled.
- `aether_mmap_smoke`: mmap smoke example executable when examples are enabled.
- `aether_wal_replay`: WAL replay example executable when examples are enabled.
- `aether_broker_basic`: broker example executable when examples are enabled.
- `aether_persistent_broker`: persistent broker example executable when examples are enabled.
- `aether_stress_spsc`: manual SPSC stress tool when tools are enabled.
- `aether_app_bench`: CLI demo benchmark target, output `aether_bench`, when apps are enabled.
- `aether_app_pub`: CLI WAL publisher target, output `aether_pub`, when apps are enabled.
- `aether_app_sub`: CLI subscriber/replay demo target, output `aether_sub`, when apps are enabled.
- `aether_app_replay`: CLI raw WAL replay target, output `aether_replay`, when apps are enabled.
- `aether_app_inspect_wal`: CLI WAL inspection target, output `aether_inspect_wal`, when apps are enabled.
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
- `aether_test_broker`: broker CTest executable when tests are enabled.
- `aether_test_persistent_broker`: persistent broker CTest executable when tests are enabled.
- `aether_test_cli_args`: CLI argument parsing CTest executable when tests are enabled.
- `aether_test_counters`: metrics counters CTest executable when tests are enabled.
- `aether_test_latency_histogram`: latency histogram CTest executable when tests are enabled.
- `aether_test_batch_broker`: batch broker CTest executable when tests are enabled.
- `aether_test_zero_copy_spsc`: zero-copy SPSC CTest executable when tests are enabled.
- `aether_bench_spsc_throughput`: SPSC throughput benchmark when benchmarks are enabled.
- `aether_bench_spsc_latency`: SPSC latency benchmark when benchmarks are enabled.
- `aether_bench_payload_sizes`: SPSC payload-size benchmark when benchmarks are enabled.
- `aether_bench_broker_end_to_end`: broker end-to-end benchmark when benchmarks are enabled.
- `aether_bench_batch_publish`: batch publish benchmark when benchmarks are enabled.
- `aether_bench_zero_copy_spsc`: zero-copy SPSC benchmark when benchmarks are enabled.
- `aether_bench_spin_wait`: spin-wait microbenchmark when benchmarks are enabled.

## Current implemented components

- Public version API.
- Core type aliases and helpers.
- Status/error handling.
- Expected-like result wrapper.
- Queue, WAL, and broker config structs with validation helpers.
- Non-owning message header/view model.
- Header-only SPSC ring buffer for exactly one producer and exactly one consumer.
- Experimental zero-copy SPSC queue (`aether::ZeroCopySpsc<T, Capacity>`).
- Cache-line and platform detail helpers.
- Clock and thread utility helpers.
- `aether::utils::cpu_relax` and `aether::utils::SpinWait`.
- CPU affinity APIs: `CpuAffinityInfo`, `cpu_affinity_info()`, `cpu_affinity_supported()`, `pin_current_thread_to_cpu()`, and `clear_current_thread_affinity()`.
- RAII POSIX memory-mapped file wrapper (`MmapFile`).
- WAL record format.
- CRC32 checksum support.
- Append-only WAL writer.
- Sequential WAL reader.
- In-memory broker API (`Broker<T, Capacity>`).
- Batch-oriented in-memory broker API (`aether::BatchBroker<T, Capacity>`).
- WAL-backed persistent broker API (`PersistentBroker<T, Capacity>`).
- WAL-before-queue publish semantics.
- Typed replay for trivially copyable persistent broker event types.
- `aether::metrics::BrokerMetricsSnapshot`.
- `aether::metrics::BrokerCounters`.
- `aether::metrics::LatencyHistogram`.
- Broker metrics APIs: `metrics_snapshot()`, `snapshot()`, and `reset_metrics()`.
- Persistent broker replay metrics through `replay_with_metrics(...)`.
- Phase 9 CLI argument parser in `include/aether/cli/args.hpp` and `src/cli/args.cpp`.
- CLI demo benchmark `aether_bench`.
- WAL publisher demo `aether_pub`.
- Local subscriber / typed WAL replay demo `aether_sub`.
- Raw WAL replay CLI `aether_replay`.
- WAL inspection CLI `aether_inspect_wal`.
- Phase 10 CLI metrics summaries across the CLI toolkit.
- WAL replay example.
- Broker examples.
- Smoke, basic SPSC, and mmap smoke examples.
- Version, status, message, SPSC basic, SPSC wraparound, SPSC concurrent, SPSC move-only, SPSC stress, mmap file, WAL, broker, batch broker, zero-copy SPSC, metrics counters, latency histogram, and CLI argument parsing tests.
- Broker API documentation.
- CLI guide documentation.
- Manual SPSC stress-validation tool.
- Google Benchmark dependency wiring gated behind `AETHER_BUILD_BENCHMARKS`.
- SPSC throughput, per-message latency, payload-size, broker end-to-end, batch publish, zero-copy SPSC, and spin-wait benchmark executables.
- Release-mode benchmark runner script with raw outputs under `benchmark-results/`.
- Benchmark methodology, performance-results template docs, Phase 12 low-latency tuning docs, and HFT-style design notes.
- GitHub Actions CI workflow at `.github/workflows/ci.yml`.
- GitHub Actions sanitizer workflow at `.github/workflows/sanitizer.yml` for ASAN/UBSAN and TSAN.
- GitHub Actions benchmark smoke workflow at `.github/workflows/benchmark-smoke.yml`.
- Moderate clang-tidy configuration in `.clang-tidy`.
- Centralized sanitizer CMake module in `cmake/AetherSanitizers.cmake`.
- CMake install/export package rules in `cmake/AetherInstall.cmake`.
- Installed package config template in `cmake/AetherStreamConfig.cmake.in`.
- Contributor guide in `CONTRIBUTING.md`.
- Unreleased change tracking in `CHANGELOG.md`.
- Pre-release verification checklist in `docs/release-checklist.md`.
- CMake options `AETHER_ENABLE_ASAN`, `AETHER_ENABLE_UBSAN`, `AETHER_ENABLE_TSAN`, `AETHER_ENABLE_CLANG_TIDY`, and `AETHER_ENABLE_INSTALL`.

## Do not add unless requested by a phase

- Measured performance claims that are not backed by raw benchmark outputs.
- MPMC queues unless explicitly requested.
- Networking, IPC broker service behavior, or live inter-process subscriptions.
- New external dependencies unless the active phase explicitly asks for them.

## Do not overclaim

Do not add fake performance numbers, production-ready claims, HFT-ready claims, or wording that implies networking, multi-producer/multi-consumer support, live inter-process broker subscriptions, production persistence, or official benchmark results are complete. Phase 11/12 provide CI, sanitizers, clang-tidy, benchmark smoke checks, CMake package install/export support, and low-latency comparison APIs/benchmarks, but these verification tools do not make the project production-ready. The manual SPSC stress tool is for correctness/stress validation only and must not be presented as a benchmark result.

## Diagram rules

- Use Mermaid diagrams directly in Markdown for architecture, ring-buffer, and WAL flow diagrams.
- Do not create a documentation image-asset directory or PNG/SVG diagram assets unless a future task explicitly asks for image files.
- Do not reference generated image files for Phase 13 diagrams.

## Style rules

- Use C++20 for C++ code.
- Use the repository `.clang-format` file for C++ formatting.
- Keep commits small and focused.
- Do not vendor dependencies unless a later phase explicitly asks for them.

## Expected future layout

Future phases may expand `include/aether/`, `src/`, `tests/`, `benchmarks/`, and `docs/` for explicitly requested future features. `.github/workflows/` already contains Phase 11 CI, sanitizer, and benchmark smoke workflows. Do not create future-phase docs or directories early unless explicitly requested.

## Local verification

Useful local commands:

```sh
./scripts/run_tests.sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON -DAETHER_BUILD_EXAMPLES=ON -DAETHER_BUILD_TOOLS=ON -DAETHER_BUILD_APPS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
ctest --test-dir build/debug --output-on-failure -R broker
ctest --test-dir build/debug --output-on-failure -R "counter|histogram|metrics"
ctest --test-dir build/debug --output-on-failure -R "batch_broker|zero_copy"
./build/debug/examples/smoke
./build/debug/examples/basic_spsc
./build/debug/examples/mmap_smoke
./build/debug/examples/wal_replay
./build/debug/examples/broker_basic
./build/debug/examples/persistent_broker
./build/debug/tools/stress_spsc --messages 1000000 --capacity 1024
./build/debug/apps/aether_bench --help
./build/debug/apps/aether_pub --help
./build/debug/apps/aether_sub --help
./build/debug/apps/aether_replay --help
./build/debug/apps/aether_inspect_wal --help
./build/debug/apps/aether_pub --wal data/sample.wal --messages 10
./build/debug/apps/aether_inspect_wal --wal data/sample.wal
./build/debug/apps/aether_replay --wal data/sample.wal --limit 3
./build/debug/apps/aether_sub --wal data/sample.wal --limit 3
./build/release/benchmarks/bench_broker_end_to_end --benchmark_min_time=0.1s
./build/release/benchmarks/bench_batch_publish --benchmark_min_time=0.1s
./build/release/benchmarks/bench_zero_copy_spsc --benchmark_min_time=0.1s
./build/release/benchmarks/bench_spin_wait --benchmark_min_time=0.1s
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s  # runs all current benchmark executables

# Phase 11/12 quality checks
./scripts/format_all.sh --check

cmake -S . -B build/asan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON -DAETHER_ENABLE_ASAN=ON -DAETHER_ENABLE_UBSAN=ON
cmake --build build/asan
ctest --test-dir build/asan --output-on-failure

cmake -S . -B build/tsan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON -DAETHER_ENABLE_TSAN=ON
cmake --build build/tsan
ctest --test-dir build/tsan --output-on-failure

cmake -S . -B build/tidy -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON -DAETHER_BUILD_EXAMPLES=ON -DAETHER_BUILD_TOOLS=ON -DAETHER_BUILD_APPS=ON -DAETHER_ENABLE_CLANG_TIDY=ON
cmake --build build/tidy

cmake -S . -B build/package -G Ninja -DCMAKE_BUILD_TYPE=Release -DAETHER_BUILD_TESTS=ON -DAETHER_ENABLE_INSTALL=ON
cmake --build build/package
ctest --test-dir build/package --output-on-failure
cmake --install build/package --prefix install/aether
```

## Cost/limit efficiency

- Read `AGENTS.md` and `docs/00-project-map.md` first before scanning the whole repository.
- Do not perform broad rewrites outside the requested phase.
- Do not fetch external dependencies unless asked.
- Keep diffs focused.
