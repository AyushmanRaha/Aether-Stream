# AGENTS.md

## Project summary

Aether-Stream is a planned C++20 ultra-low-latency lock-free asynchronous message broker.

## Current phase

The repository is in Phase 0 baseline only.

## Phase boundaries

- Phase 0: documentation, setup, and configuration only.
- Phase 1: real CMake library target.
- Phase 2+: code implementation.

Do not add library code, broker APIs, ring buffers, WAL implementation, benchmarks, tests, examples, or CLI tools while working on Phase 0 tasks.

## Do not overclaim

Do not add fake performance numbers, MVP claims, production-ready claims, or wording that implies the broker, SPSC queue, WAL, benchmarks, or CLI already works.

## Style rules

- Use C++20 when C++ code is introduced in later phases.
- Use the repository `.clang-format` file for C++ formatting.
- Keep commits small and focused.
- Do not vendor dependencies unless a later phase explicitly asks for them.

## Expected future layout

Future phases may add `include/`, `src/`, `tests/`, `benchmarks/`, `apps/`, `examples/`, `tools/`, `cmake/`, `docs/`, and `scripts/` as needed.

## Local verification

Useful local commands:

```sh
chmod +x scripts/bootstrap_macos.sh
./scripts/bootstrap_macos.sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

The CMake command may configure the Phase 0 placeholder project, but it should not be treated as full build validation yet.

## Cost/limit efficiency

- Read `AGENTS.md` and `docs/00-project-map.md` first before scanning the whole repository.
- Do not perform broad rewrites outside the requested phase.
- Do not fetch external dependencies unless asked.
- Keep diffs focused.
