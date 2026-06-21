# Project Map

## Current Phase 2 layout

The repository now contains the Phase 2 build skeleton, minimal library pipeline, and reusable core public types:

- `README.md` explains the project goal, current Phase 2 status, roadmap, and build/test commands.
- `CMakeLists.txt` configures the real CMake project and defines the `aether_stream` library target.
- `LICENSE` contains the MIT license for the project.
- `.gitignore` excludes local build outputs, logs, persistence files, cache files, and editor artifacts.
- `.editorconfig` keeps whitespace, line endings, and indentation consistent across editors.
- `.clang-format` defines the C++ formatting style.
- `AGENTS.md` gives coding agents concise project context, current targets, and phase boundaries.
- `cmake/` contains reusable CMake modules:
  - `AetherOptions.cmake` defines developer-mode defaults and build options.
  - `AetherCompilerWarnings.cmake` applies project warning flags to selected targets.
  - `AetherDependencies.cmake` reserves a dependency-wiring location without fetching dependencies in Phase 2.
- `include/aether/version.hpp` exposes the public version constants and `version_string()` API.
- `include/aether/core/types.hpp` defines stable core aliases, constants, and a power-of-two helper.
- `include/aether/core/status.hpp` declares status codes and the lightweight `Status` type.
- `include/aether/core/expected.hpp` provides a small C++20 expected-like result wrapper.
- `include/aether/core/config.hpp` defines queue, WAL, and broker config structs with validation helpers.
- `include/aether/message.hpp` defines the non-owning message header/view model and validation helpers.
- `src/version.cpp` compiles the version API into the library target.
- `src/core/status.cpp` compiles stable status names and default messages into the library target.
- `examples/smoke.cpp` is a tiny executable that links against `aether::stream` and prints the version.
- `tests/CMakeLists.txt` builds and registers standalone CTest executables.
- `tests/test_version.cpp` checks the public version constants and string without GoogleTest.
- `tests/test_status.cpp` checks status and expected-like result behavior without GoogleTest.
- `tests/test_message.cpp` checks message view construction and validation without GoogleTest.
- `scripts/bootstrap_macos.sh` checks local macOS development tools.
- `scripts/run_tests.sh` configures, builds, and runs CTest locally.
- `scripts/format_all.sh` formats or checks C++ files in source directories.

## Current build pipeline

The Phase 2 pipeline is intentionally small:

1. CMake configures the project with C++20 and local options.
2. The `aether_stream` library compiles `src/version.cpp` and `src/core/status.cpp`.
3. The `aether_smoke` example links against `aether::stream` when examples are enabled.
4. The `aether_test_version`, `aether_test_status`, and `aether_test_message` executables link against `aether::stream` when tests are enabled.
5. CTest runs the registered `aether.version`, `aether.status`, and `aether.message` tests.

## Still not present yet

Phase 2 does not include:

- an SPSC queue;
- WAL or `mmap` persistence;
- a broker API;
- benchmarks;
- CLI apps;
- CI automation.

Phase 3 will add the first SPSC ring buffer work.

## Planned future layout

Later phases are expected to add or expand these directories when they are needed:

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

## Phase boundaries

Phase 0 established documentation, setup, and configuration. Phase 1 created the real CMake skeleton and first library/test/example path. Phase 2 added core public types, status/error handling, an expected-like wrapper, config structs, and the non-owning message model. Phase 3 and later phases will begin implementing concurrency primitives, persistence, broker behavior, tooling, tests, benchmarks, and packaging.
