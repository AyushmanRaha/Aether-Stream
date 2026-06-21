# AGENTS.md

## Project summary

Aether-Stream is a planned C++20 ultra-low-latency lock-free asynchronous message broker.

## Current phase

Phase 2 has been implemented: the repository now has core public types, status/error handling, an expected-like wrapper, config structs, a message header/view model, and version/status/message CTest tests.

## Next phase

Phase 3 is next: SPSC ring buffer v1.

## Phase boundaries

- Phase 0 completed: documentation, setup, and configuration.
- Phase 1 completed: CMake skeleton, library target, version API, smoke example, and first CTest test.
- Phase 2 completed: core public types, message model, and error/status handling.
- Phase 3 next: SPSC ring buffer v1.
- Phase 4+ later: concurrency correctness hardening, persistence, broker behavior, benchmarks, CLI tools, CI, packaging, and advanced tuning.

Do not add Phase 3+ implementation unless the active task explicitly asks for that phase.

## Current build targets

- `aether_stream`: main library target.
- `aether::stream`: alias target for consumers.
- `aether_smoke`: smoke example executable when examples are enabled.
- `aether_test_version`: version CTest executable when tests are enabled.
- `aether_test_status`: status CTest executable when tests are enabled.
- `aether_test_message`: message CTest executable when tests are enabled.

## Do not add unless requested by a phase

- SPSC queue
- WAL
- `mmap` persistence
- broker implementation or broker APIs
- benchmarks
- CLI apps
- sanitizer files
- packaging/export/install logic
- external dependencies

## Do not overclaim

Do not add fake performance numbers, MVP claims, production-ready claims, or wording that implies the broker, SPSC queue, WAL, benchmarks, or CLI already works.

## Style rules

- Use C++20 for C++ code.
- Use the repository `.clang-format` file for C++ formatting.
- Keep commits small and focused.
- Do not vendor dependencies unless a later phase explicitly asks for them.

## Expected future layout

Future phases may add or expand `include/`, `src/`, `tests/`, `benchmarks/`, `apps/`, `examples/`, `tools/`, `cmake/`, `docs/`, and `scripts/` as needed.

## Local verification

Useful local commands:

```sh
./scripts/run_tests.sh
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_TESTS=ON
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
./build/debug/examples/smoke
```

## Cost/limit efficiency

- Read `AGENTS.md` and `docs/00-project-map.md` first before scanning the whole repository.
- Do not perform broad rewrites outside the requested phase.
- Do not fetch external dependencies unless asked.
- Keep diffs focused.
