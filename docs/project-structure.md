# Expanded project structure

This is a documentation view of the source-controlled repository layout. Generated build directories, local benchmark output directories, temporary files, dependency caches, IDE settings, compiler outputs, and `.git/` metadata are intentionally excluded.

```text
Aether-Stream/
├── .github/
│   ├── workflows/
│   │   ├── benchmark-smoke.yml
│   │   ├── ci.yml
│   │   └── sanitizer.yml
│   └── CODEOWNERS
├── apps/
│   ├── aether_bench.cpp
│   ├── aether_inspect_wal.cpp
│   ├── aether_pub.cpp
│   ├── aether_replay.cpp
│   └── aether_sub.cpp
├── benchmarks/
│   ├── CMakeLists.txt
│   ├── bench_batch_publish.cpp
│   ├── bench_broker_end_to_end.cpp
│   ├── bench_payload_sizes.cpp
│   ├── bench_spin_wait.cpp
│   ├── bench_spsc_latency.cpp
│   ├── bench_spsc_throughput.cpp
│   └── bench_zero_copy_spsc.cpp
├── cmake/
│   ├── AetherCompilerWarnings.cmake
│   ├── AetherDependencies.cmake
│   ├── AetherInstall.cmake
│   ├── AetherOptions.cmake
│   ├── AetherSanitizers.cmake
│   └── AetherStreamConfig.cmake.in
├── docs/
│   ├── benchmark-results/
│   │   └── m1-macbook-air-2026-06-29.md
│   ├── architecture.md
│   ├── benchmark-methodology.md
│   ├── broker-api.md
│   ├── cli-guide.md
│   ├── concepts-guide.md
│   ├── limitations.md
│   ├── low-latency-design-notes.md
│   ├── low-latency-tuning.md
│   ├── memory-ordering.md
│   ├── metrics.md
│   ├── mmap-notes.md
│   ├── performance-results.md
│   ├── project-structure.md
│   ├── release-checklist.md
│   ├── repository-guide.md
│   ├── ring-buffer-design.md
│   └── wal-format.md
├── examples/
│   ├── basic_spsc.cpp
│   ├── broker_basic.cpp
│   ├── mmap_smoke.cpp
│   ├── persistent_broker.cpp
│   ├── smoke.cpp
│   └── wal_replay.cpp
├── include/
│   └── aether/
│       ├── cli/
│       │   └── args.hpp
│       ├── core/
│       │   ├── config.hpp
│       │   ├── expected.hpp
│       │   ├── status.hpp
│       │   └── types.hpp
│       ├── detail/
│       │   ├── cache_line.hpp
│       │   └── platform.hpp
│       ├── io/
│       │   └── mmap_file.hpp
│       ├── metrics/
│       │   ├── counters.hpp
│       │   ├── latency_histogram.hpp
│       │   └── snapshot.hpp
│       ├── utils/
│       │   ├── clock.hpp
│       │   ├── cpu_affinity.hpp
│       │   ├── spin_wait.hpp
│       │   └── thread_utils.hpp
│       ├── wal/
│       │   ├── checksum.hpp
│       │   ├── record.hpp
│       │   ├── wal_reader.hpp
│       │   └── wal_writer.hpp
│       ├── batch_broker.hpp
│       ├── broker.hpp
│       ├── message.hpp
│       ├── persistent_broker.hpp
│       ├── spsc_ring_buffer.hpp
│       ├── version.hpp
│       └── zero_copy_spsc.hpp
├── scripts/
│   ├── bootstrap_macos.sh
│   ├── format_all.sh
│   ├── run_benchmarks.sh
│   └── run_tests.sh
├── src/
│   ├── cli/
│   │   └── args.cpp
│   ├── core/
│   │   └── status.cpp
│   ├── io/
│   │   └── mmap_file.cpp
│   ├── metrics/
│   │   └── latency_histogram.cpp
│   ├── utils/
│   │   └── cpu_affinity.cpp
│   ├── wal/
│   │   ├── checksum.cpp
│   │   ├── wal_reader.cpp
│   │   └── wal_writer.cpp
│   ├── broker.cpp
│   └── version.cpp
├── tests/
│   ├── CMakeLists.txt
│   ├── test_batch_broker.cpp
│   ├── test_broker.cpp
│   ├── test_cli_args.cpp
│   ├── test_counters.cpp
│   ├── test_latency_histogram.cpp
│   ├── test_message.cpp
│   ├── test_mmap_file.cpp
│   ├── test_persistent_broker.cpp
│   ├── test_spsc_basic.cpp
│   ├── test_spsc_concurrent.cpp
│   ├── test_spsc_move_only.cpp
│   ├── test_spsc_stress.cpp
│   ├── test_spsc_wraparound.cpp
│   ├── test_status.cpp
│   ├── test_version.cpp
│   ├── test_wal_reader.cpp
│   ├── test_wal_record.cpp
│   ├── test_wal_writer.cpp
│   └── test_zero_copy_spsc.cpp
├── tools/
│   └── stress_spsc.cpp
├── .clang-format
├── .clang-tidy
├── .editorconfig
├── .gitignore
├── AGENTS.md
├── CHANGELOG.md
├── CMakeLists.txt
├── CONTRIBUTING.md
├── LICENSE
├── README.md
└── RELEASE_NOTES_v0.1.0.md
```

Notes:

- `.github/workflows/` contains the source-controlled CI, sanitizer, and benchmark-smoke workflows.
- `include/aether/` contains the public C++ headers installed for consumers.
- `src/`, `apps/`, `examples/`, `tools/`, `tests/`, and `benchmarks/` contain implementation files, CLI demos, examples, manual tools, CTest executables, and benchmark executables respectively.
- `cmake/` and `scripts/` contain reusable build modules and local verification helpers.
