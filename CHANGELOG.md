# Changelog

All notable changes to Aether-Stream will be documented in this file.

The format is based on Keep a Changelog style, and this project is moving toward an initial v0.1.0 release.

## [Unreleased]

### Added

- Final portfolio README rewrite with centered hero, badge rows, navigation, inline Mermaid diagrams, and honest status notes.
- Deep architecture documentation in `docs/architecture.md`.
- Mermaid diagrams replacing generated diagram image assets; no PNG diagrams are required for Phase 13.
- Honest limitations document in `docs/limitations.md`.
- Interview preparation notes in `docs/interview-notes.md`.
- v0.1.0 candidate release notes in `RELEASE_NOTES_v0.1.0.md`.
- Benchmark reporting polish for methodology and performance-results documentation.

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

- Project map and agent guidance now reflect completion through Phase 13 documentation/portfolio packaging.
- Ring-buffer, memory-ordering, WAL, benchmark methodology, performance results, and release checklist docs polished for Phase 13.
- Benchmark runner now includes Phase 12 benchmark executables.
- CMake now supports build/install interface include directories.
