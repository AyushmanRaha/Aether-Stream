# Changelog

All notable changes to Aether-Stream will be documented in this file.

The format is based on Keep a Changelog style, and this project is moving toward an initial v0.1.0 release.

## [Unreleased]

### Added

- Batch broker API with batch publish/consume support.
- Experimental zero-copy SPSC queue with reservation/construct/commit/cancel workflow.
- Spin-wait utility and portable `cpu_relax` helper.
- Linux-first CPU affinity helper with macOS/unsupported-platform safe fallback.
- Batch publish benchmark.
- Zero-copy SPSC benchmark.
- Spin-wait microbenchmark.
- Low-latency tuning documentation.
- HFT-style design notes.
- GitHub Actions CI workflow for Ubuntu/macOS Debug and Release builds.
- Sanitizer workflow for ASAN/UBSAN and TSAN.
- Benchmark smoke workflow.
- CMake sanitizer configuration module.
- CMake install/export package support.
- Moderate clang-tidy configuration.
- Contributor guide.
- Release checklist.

### Changed

- Benchmark runner now includes Phase 12 benchmark executables.
- README, project map, and agent guidance updated to reflect Phase 12 as implemented.
- CMake now supports build/install interface include directories.
- README now reflects the current Phase 12 status while preserving Phase 11 quality automation notes.
