# Release Checklist

This checklist is for pre-tag verification before v0.1.0 or any later Aether-Stream release.

## Required local checks

- Run `./scripts/format_all.sh --check`.
- Configure, build, and test Debug with CTest.
- Configure, build, and test Release with CTest.
- Configure, build, and test ASAN/UBSAN.
- Configure, build, and test TSAN separately from ASAN.
- Run benchmark smoke checks.
- Run package install smoke checks and verify a temporary consumer can use `find_package(AetherStream CONFIG REQUIRED)` and link `aether::stream`.

## Documentation checks

- Confirm `README.md` reflects the current codebase and keeps limitations prominent.
- Confirm README Mermaid diagrams render on GitHub.
- Confirm links to `docs/repository-guide.md`, `docs/concepts-guide.md`, and all other docs resolve.
- Confirm `docs/cli-guide.md` matches current CLI behavior.
- Confirm `docs/benchmark-methodology.md` matches the current benchmark workflow.
- Confirm `docs/performance-results.md` contains only raw-output-backed measurements or empty placeholders.
- Confirm limitations do not imply production readiness, networking, MPMC support, native Windows verification, or unsupported latency guarantees.

## Release steps

1. Update the project version if required.
2. Update `CHANGELOG.md` for the release.
3. Run all final local and CI checks.
4. Create an annotated git tag.
5. Push the tag.

## Benchmark publishing warning

Do not publish official benchmark numbers unless they were generated from `scripts/run_benchmarks.sh` with environment details and raw outputs preserved.
