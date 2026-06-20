# Project Map

## Current Phase 0 layout

The repository is intentionally small in Phase 0:

- `README.md` explains the project goal, current status, roadmap, and setup commands.
- `CMakeLists.txt` is a minimal placeholder that records the project name and C++20 baseline.
- `LICENSE` contains the MIT license for the project.
- `.gitignore` excludes local build outputs, logs, persistence files, and editor artifacts.
- `.editorconfig` keeps whitespace, line endings, and indentation consistent across editors.
- `.clang-format` defines the future C++ formatting style.
- `AGENTS.md` gives coding agents concise project context and phase boundaries.
- `docs/` contains project documentation for humans and future contributors.
- `scripts/` contains local helper scripts, starting with the macOS bootstrap check.

## Planned future layout

Later phases are expected to add these directories when they are needed:

- `include/aether/` for public C++ library headers.
- `src/` for library implementation files.
- `tests/` for unit, integration, and stress tests.
- `benchmarks/` for latency and throughput benchmarks.
- `examples/` for small usage examples.
- `apps/` for broker or CLI applications.
- `tools/` for developer and diagnostic tools.
- `cmake/` for reusable CMake modules.
- `docs/` for design notes, learning material, and user documentation.
- `scripts/` for local setup and maintenance scripts.
- `.github/workflows/` for CI automation.

## What not to expect yet

Phase 0 does not include:

- a library target;
- tests;
- benchmarks;
- a broker implementation.

## Phase boundaries

Phase 0 is documentation, setup, and configuration only. It establishes a truthful baseline without claiming that the broker or library exists.

Phase 1 will create the real CMake skeleton and first library target. Phase 2 and later phases will begin implementing C++ types, concurrency primitives, persistence, broker behavior, tooling, tests, benchmarks, and packaging.
