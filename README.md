# Aether-Stream

Aether-Stream is a planned C++20 ultra-low-latency lock-free asynchronous message broker project.

## Current status

This repository is currently in **Phase 0: repository baseline and local development foundation**.

No production-ready broker, SPSC queue, write-ahead log (WAL), benchmarks, or CLI exists yet. The current goal is to keep the repository honest, understandable, and ready for later implementation phases.

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

The Phase 0 repository contains:

- baseline repository documentation;
- formatting and editor configuration;
- a bootstrap script for macOS development checks;
- a minimal placeholder CMake file.

## What does not exist yet

The repository does **not** currently include:

- a real library target;
- benchmarks;
- performance numbers;
- a persistence layer;
- a production-ready API.

## Planned roadmap

0. Repo baseline and local environment
1. CMake skeleton and library target
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

## Future build commands

The following commands are intended for **Phase 1+** once real CMake targets and tests exist:

```sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
```

During Phase 0, CMake only configures a placeholder project.

## Repository docs

- [Project map](docs/00-project-map.md)
- [Learning roadmap](docs/01-learning-roadmap.md)

## License

MIT.
