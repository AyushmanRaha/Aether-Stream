# AGENTS.md

## Project summary

Aether-Stream is a C++20 ultra-low-latency lock-free asynchronous message broker library under development. The current repository is complete through Phase 10: it includes the reusable foundation, SPSC queue, mmap layer, WAL writer/reader, in-memory broker API, WAL-backed persistent broker API, Phase 9 CLI toolkit, and Phase 10 metrics/diagnostics/observability. It is not production-ready and does not yet include CI, packaging, or advanced low-latency APIs.

## Current phase

Phase 10 has been completed. The repository includes the Phase 0 setup, Phase 1 CMake/library/test skeleton, Phase 2 core public types and message model, Phase 3 SPSC ring buffer v1, Phase 4 SPSC concurrency-correctness hardening, Phase 5 benchmark framework and honest performance-reporting docs, Phase 6 memory-mapped file abstraction, Phase 7 WAL writer/reader persistence foundation, Phase 8 broker integration with in-memory broker API, persistent broker API, WAL-before-queue semantics, typed replay, broker examples, broker tests, and broker API docs, plus Phase 9 CLI argument parsing helpers, five CLI apps, CLI args tests, CLI guide, README demo flow, Phase 10 metrics snapshots, relaxed-atomic counters, latency histogram, CLI metrics output, metrics docs, and broker end-to-end benchmark.

## Next phase

Phase 11 is next: CI, sanitizers, static analysis, and packaging. Do not add Phase 11+ implementation unless the active task explicitly asks for that phase.

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
- Phase 11+ later: CI, packaging, release work, and advanced tuning.

Do not add Phase 11+ implementation unless the active task explicitly asks for that phase.

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
- `aether_bench_spsc_throughput`: SPSC throughput benchmark when benchmarks are enabled.
- `aether_bench_spsc_latency`: SPSC latency benchmark when benchmarks are enabled.
- `aether_bench_payload_sizes`: SPSC payload-size benchmark when benchmarks are enabled.
- `aether_bench_broker_end_to_end`: broker end-to-end benchmark when benchmarks are enabled.

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
- In-memory broker API (`Broker<T, Capacity>`).
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
- Version, status, message, SPSC basic, SPSC wraparound, SPSC concurrent, SPSC move-only, SPSC stress, mmap file, WAL, broker, metrics counters, latency histogram, and CLI argument parsing tests.
- Broker API documentation.
- CLI guide documentation.
- Manual SPSC stress-validation tool.
- Google Benchmark dependency wiring gated behind `AETHER_BUILD_BENCHMARKS`.
- SPSC throughput, per-message latency, payload-size, and broker end-to-end benchmark executables.
- Release-mode benchmark runner script with raw outputs under `benchmark-results/`.
- Benchmark methodology and performance-results template docs.

## Do not add unless requested by a phase

- Measured performance claims that are not backed by raw benchmark outputs.
- GitHub Actions CI, sanitizer job files, packaging, export/install logic, or release automation.
- External dependencies.

## Do not overclaim

Do not add fake performance numbers, MVP claims, production-ready claims, HFT-ready claims, or wording that implies CI, packaging, networking, multi-producer/multi-consumer support, live inter-process broker subscriptions, or production persistence is complete. Phase 10 provides local terminal demos, WAL inspection/replay tools, and in-process metrics/diagnostics only. The manual SPSC stress tool is for correctness/stress validation only and must not be presented as a benchmark result.

## Style rules

- Use C++20 for C++ code.
- Use the repository `.clang-format` file for C++ formatting.
- Keep commits small and focused.
- Do not vendor dependencies unless a later phase explicitly asks for them.

## Expected future layout

Future phases may expand `tools/`, `cmake/`, `docs/`, `scripts/`, and `.github/workflows/` as needed. `apps/` already contains the Phase 9 CLI toolkit with Phase 10 metrics output. Do not create future-phase docs or directories early unless explicitly requested.

## Local verification

Useful local commands:

```sh
./scripts/run_tests.sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON -DAETHER_BUILD_EXAMPLES=ON -DAETHER_BUILD_TOOLS=ON -DAETHER_BUILD_APPS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
ctest --test-dir build/debug --output-on-failure -R broker
ctest --test-dir build/debug --output-on-failure -R "counter|histogram|metrics"
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
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s
```

## Cost/limit efficiency

- Read `AGENTS.md` and `docs/00-project-map.md` first before scanning the whole repository.
- Do not perform broad rewrites outside the requested phase.
- Do not fetch external dependencies unless asked.
- Keep diffs focused.
