# Release checklist

This checklist is for pre-tag verification before v0.1.0 or any later Aether-Stream release.

## Required local checks

- Run the format check: `./scripts/format_all.sh --check`.
- Configure, build, and test Debug with CTest.
- Configure, build, and test Release with CTest.
- Configure, build, and test ASAN/UBSAN.
- Configure, build, and test TSAN separately from ASAN.
- Run the benchmark smoke build and short benchmark executable checks.
- Run the package install smoke check and verify a temporary consumer can use `find_package(AetherStream CONFIG REQUIRED)` and link `aether::stream`.

## Documentation checks

- Confirm `README.md` reflects completion through Phase 13 and keeps limitations prominent.
- Confirm README Mermaid diagrams render on GitHub.
- Confirm `docs/architecture.md` exists.
- Confirm `docs/limitations.md` exists.
- Confirm `docs/interview-notes.md` exists.
- Confirm `RELEASE_NOTES_v0.1.0.md` exists.
- Confirm `docs/cli-guide.md` matches the current CLI behavior.
- Confirm `docs/benchmark-methodology.md` matches the current benchmark workflow.
- Confirm `docs/performance-results.md` contains only measured numbers backed by raw outputs.
- Confirm limitations remain honest and do not imply production readiness, networking, MPMC support, or unsupported latency guarantees.

## Release steps

1. Update the project version if required.
2. Update `CHANGELOG.md` for the release.
3. Run all final local and CI checks.
4. Create an annotated git tag.
5. Push the tag.

## Benchmark publishing warning

Do not publish official benchmark numbers unless they were generated from `scripts/run_benchmarks.sh` with environment details and raw outputs preserved.
