# Aether-Stream

Aether-Stream is a planned C++20 ultra-low-latency lock-free asynchronous message broker project.

## Current status

This repository has completed **Phase 1: CMake skeleton and real library target**.

The project now has a real CMake library target, a public version API, a compiled version source file, a smoke example, and one CTest-based version test. It is still an early skeleton: no production-ready broker, SPSC queue, write-ahead log (WAL), benchmarks, or CLI exists yet.

## What this project will become

Aether-Stream is planned to grow into a C++20 message broker/library and CLI toolkit with:

- a C++20 library;
- an SPSC lock-free ring buffer;
- a clear message model;
- an optional `mmap`-backed write-ahead log;
- a broker API;
- CLI tools;
- tests, benchmarks, documentation, and CI.

## What exists today

The Phase 1 repository contains:

- a real CMake project skeleton;
- the `aether_stream` library target;
- the `aether::stream` alias target for consumers;
- a public version header at `include/aether/version.hpp`;
- a compiled version source at `src/version.cpp`;
- a smoke example executable that prints the project version;
- a first CTest test executable for the version API;
- local scripts for running tests and formatting C++ files;
- baseline repository documentation, formatting, and editor configuration.

## What does not exist yet

The repository does **not** currently include:

- a message model;
- status or error types;
- an SPSC ring buffer;
- WAL or `mmap` persistence;
- a broker API;
- a benchmark framework;
- a CLI toolkit;
- production readiness or performance claims.

## Build and test

Configure, build, test, and run the smoke example with:

```sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
./build/debug/examples/smoke
```

The smoke example should print:

```text
Aether-Stream 0.1.0
```

You can also use the local test shortcut:

```sh
./scripts/run_tests.sh
```

## Formatting

Format C++ source files in known source directories with:

```sh
./scripts/format_all.sh
```

Check formatting without modifying files with:

```sh
./scripts/format_all.sh --check
```

## Planned roadmap

0. Repo baseline and local environment — completed
1. CMake skeleton and library target — completed
2. Core types and message model
3. SPSC ring buffer v1
4. Concurrency correctness hardening
5. Benchmark framework
6. Memory-mapped file layer
7. WAL writer/reader
8. Broker integration
9. CLI toolkit and examples
10. Metrics and diagnostics
11. CI, sanitizers, packaging
12. Advanced low-latency upgrades
13. Documentation and portfolio packaging

## Local setup

For macOS development checks, run:

```sh
chmod +x scripts/bootstrap_macos.sh
./scripts/bootstrap_macos.sh
```

The script checks for local tools and creates lightweight build directories. It does not install dependencies automatically.

## Repository docs

- [Project map](docs/00-project-map.md)
- [Learning roadmap](docs/01-learning-roadmap.md)

## License

MIT.
