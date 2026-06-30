# Changelog

All notable changes to Aether-Stream will be documented in this file.

The format follows Keep a Changelog style. The repository is moving toward an initial v0.1.0 release.

## [Unreleased]

### Added

- Batch broker API with batch publish/consume support.
- Experimental zero-copy SPSC queue with reservation, construction, commit, and cancel workflow.
- Spin-wait utility and portable `cpu_relax` helper.
- CPU affinity helper with platform-specific support reporting.
- Benchmark executables for SPSC, broker, batch, zero-copy, and spin-wait scenarios.
- Metrics counters, immutable snapshots, latency histogram, and CLI metrics summaries.
- CLI applications for local broker demos, WAL publishing, replay, and inspection.
- Documentation covering architecture, broker APIs, WAL format, metrics, limitations, benchmarks, low-latency tuning, and release verification.
- v0.1.0 candidate release notes.

### Changed

- README and repository-facing documentation now describe the current codebase directly and keep limitations prominent.
- Documentation filenames now use professional current-state names rather than numbered build-history names.
- Benchmark documentation now keeps official results empty until raw outputs and environment metadata are committed.
- CMake build output now uses current-state wording.

### Build and CI

- GitHub Actions workflows cover CI builds, sanitizer builds, and benchmark smoke checks.
- CMake supports optional tests, examples, tools, apps, benchmarks, sanitizers, clang-tidy, and install/export package rules.

### Documentation

- Repository guide, concepts guide, low-latency tuning notes, and low-latency design notes are current-state references.
- Release checklist emphasizes local verification, benchmark honesty, and link hygiene.
