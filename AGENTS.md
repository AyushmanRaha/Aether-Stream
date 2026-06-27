# AGENTS.md

## Project summary

Aether-Stream is a planned C++20 ultra-low-latency lock-free asynchronous message broker library and eventual CLI toolkit. The current repository is a library foundation with an SPSC queue primitive; it is not yet a complete broker.

## Current phase

Phase 5 has been completed. The repository includes the Phase 0 setup, Phase 1 CMake/library/test skeleton, Phase 2 core public types and message model, Phase 3 SPSC ring buffer v1, Phase 4 SPSC concurrency-correctness hardening, and Phase 5 benchmark framework and honest performance-reporting docs.

## Next phase

Phase 6 is next: memory-mapped file abstraction. Do not add Phase 6 implementation unless the active task explicitly asks for that phase.

## Phase boundaries

- Phase 0 completed: documentation, setup, and configuration.
- Phase 1 completed: CMake project, library target, alias target, version API, smoke example, first CTest test, and local scripts.
- Phase 2 completed: core public types, message model, status/error handling, expected-like wrapper, and config structs.
- Phase 3 completed: header-only SPSC ring buffer v1, cache-line/platform helpers, basic SPSC example, and SPSC basic/wraparound tests.
- Phase 4 completed: SPSC hardening with move support, `size_approx`, acquire/release memory-order comments, queue tuning fields, utility helpers, concurrent/move-only/stress tests, and manual SPSC stress tool.
- Phase 5 completed: Google Benchmark wiring, SPSC throughput/latency/payload-size benchmarks, release benchmark runner, raw-result workflow, benchmark methodology doc, and performance-results template.
- Phase 6+ later: mmap file abstraction, persistence, WAL, broker behavior, CLI tools, metrics/diagnostics, CI, packaging, release work, and advanced tuning.

Do not add Phase 6+ implementation unless the active task explicitly asks for that phase.

## Current build targets

- `aether_stream`: main library target.
- `aether::stream`: alias target for consumers.
- `aether_smoke`: smoke example executable when examples are enabled.
- `aether_basic_spsc`: basic SPSC example executable when examples are enabled.
- `aether_stress_spsc`: manual SPSC stress tool when tools are enabled.
- `aether_test_version`: version CTest executable when tests are enabled.
- `aether_test_status`: status CTest executable when tests are enabled.
- `aether_test_message`: message CTest executable when tests are enabled.
- `aether_test_spsc_basic`: basic SPSC CTest executable when tests are enabled.
- `aether_test_spsc_wraparound`: wraparound SPSC CTest executable when tests are enabled.
- `aether_test_spsc_concurrent`: concurrent ordered-transfer SPSC CTest executable when tests are enabled.
- `aether_test_spsc_move_only`: move-only payload SPSC CTest executable when tests are enabled.
- `aether_test_spsc_stress`: multi-capacity SPSC stress CTest executable when tests are enabled.
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
- Smoke and basic SPSC examples.
- Version, status, message, SPSC basic, SPSC wraparound, SPSC concurrent, SPSC move-only, and SPSC stress tests.
- Manual SPSC stress-validation tool.
- Google Benchmark dependency wiring gated behind `AETHER_BUILD_BENCHMARKS`.
- SPSC throughput, per-message latency, and payload-size benchmark executables.
- Release-mode benchmark runner script with raw outputs under `benchmark-results/`.
- Benchmark methodology and performance-results template docs.

## Do not add unless requested by a phase

- Measured performance claims that are not backed by raw benchmark outputs.
- mmap file abstraction or persistence layers.
- WAL implementation.
- Broker implementation or broker APIs.
- CLI toolkit apps.
- Metrics or diagnostics systems.
- GitHub Actions CI, sanitizer job files, packaging, export/install logic, or release automation.
- External dependencies.

## Do not overclaim

Do not add fake performance numbers, MVP claims, production-ready claims, HFT-ready claims, or wording that implies the broker, WAL, CLI toolkit, metrics, CI, packaging, or persistence already works. The manual SPSC stress tool is for correctness/stress validation only and must not be presented as a benchmark result.

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
./build/debug/tools/stress_spsc --messages 1000000 --capacity 1024
./scripts/run_benchmarks.sh --benchmark_min_time=0.5s
```

## Cost/limit efficiency

- Read `AGENTS.md` and `docs/00-project-map.md` first before scanning the whole repository.
- Do not perform broad rewrites outside the requested phase.
- Do not fetch external dependencies unless asked.
- Keep diffs focused.
