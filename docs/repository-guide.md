# Repository Guide

Aether-Stream is a local C++20 systems project with a reusable library, examples, CLI tools, tests, benchmarks, and documentation. This guide maps the current repository without build-history framing.

## Top-level files

- `README.md`: main project overview, local setup instructions, architecture summary, limitations, and benchmark policy.
- `AGENTS.md`: guidance for coding agents working in this repository.
- `CONTRIBUTING.md`: local development and contribution workflow.
- `CHANGELOG.md`: unreleased change summary.
- `RELEASE_NOTES_v0.1.0.md`: draft release notes.
- `CMakeLists.txt`: root CMake configuration for the library and optional build groups.

## Source layout

- `include/aether/`: public C++20 headers.
- `src/`: non-header implementation files.
- `apps/`: CLI applications: `aether_bench`, `aether_pub`, `aether_sub`, `aether_replay`, and `aether_inspect_wal`.
- `examples/`: small runnable examples: `smoke`, `basic_spsc`, `mmap_smoke`, `wal_replay`, `broker_basic`, and `persistent_broker`.
- `tests/`: standalone CTest executables.
- `benchmarks/`: benchmark executables for queue, broker, batch, zero-copy, payload-size, and spin-wait paths.
- `tools/`: manual validation tools.
- `scripts/`: local format, test, bootstrap, and benchmark helpers.
- `.github/workflows/`: CI, sanitizer, and benchmark-smoke workflows.
- `cmake/`: reusable CMake modules for options, dependencies, warnings, sanitizers, and install/export support.

## Main build targets

- `aether_stream`: main library target.
- `aether::stream`: public alias target for consumers.
- Optional examples, tools, apps, tests, and benchmark targets are controlled by CMake options.

## CMake options

- `AETHER_BUILD_TESTS`
- `AETHER_BUILD_EXAMPLES`
- `AETHER_BUILD_TOOLS`
- `AETHER_BUILD_APPS`
- `AETHER_BUILD_BENCHMARKS`
- `AETHER_ENABLE_ASAN`
- `AETHER_ENABLE_UBSAN`
- `AETHER_ENABLE_TSAN`
- `AETHER_ENABLE_CLANG_TIDY`
- `AETHER_ENABLE_INSTALL`

## Useful commands

```sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
```

```sh
./scripts/format_all.sh --check
./scripts/run_benchmarks.sh
```

## Documentation map

- `docs/concepts-guide.md`: conceptual primer.
- `docs/architecture.md`: component overview and Mermaid diagrams.
- `docs/ring-buffer-design.md`: SPSC queue design.
- `docs/memory-ordering.md`: acquire/release explanation.
- `docs/mmap-notes.md`: mapped file abstraction.
- `docs/wal-format.md`: WAL record format.
- `docs/broker-api.md`: broker APIs.
- `docs/cli-guide.md`: CLI tools.
- `docs/metrics.md`: counters, snapshots, and histograms.
- `docs/benchmark-methodology.md`: benchmark workflow and publication rules.
- `docs/performance-results.md`: summarized benchmark results.
- `docs/low-latency-tuning.md`: tuning notes.
- `docs/low-latency-design-notes.md`: design tradeoffs.
- `docs/limitations.md`: explicit non-goals.
- `docs/benchmark-results/m1-macbook-air-2026-06-29.md`: consolidated redacted benchmark evidence.
- `docs/release-checklist.md`: pre-release checks.
