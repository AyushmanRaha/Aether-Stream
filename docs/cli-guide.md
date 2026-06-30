# Aether-Stream Setup and CLI Guide

## What this guide covers

This guide walks from a fresh machine to full local verification of Aether-Stream. It covers how to:

- install prerequisites
- clone the repository
- build a Debug configuration
- run CTest tests
- run examples
- run the manual stress tool
- build Release CLI apps
- run every CLI app
- understand the important output fields
- run the formatting check
- run sanitizer builds
- run a package/install smoke test
- run benchmarks
- clean generated outputs for a rebuild
- troubleshoot common warnings and setup problems

Aether-Stream is intentionally local-only:

- no networking
- no daemon
- no live remote subscriber
- no production broker claims

The CLI tools demonstrate local queue, broker, WAL, replay, and metrics behavior. They are useful for learning and validation, but they are not network clients or production services.

## Command map

| Area | Command/Target | Purpose |
|---|---|---|
| Clone | `git clone ...` | Download the repository |
| Configure | `cmake -S . -B ...` | Generate build files |
| Build | `cmake --build ...` | Compile library/apps/tests |
| Test | `ctest --test-dir ...` | Run automated checks |
| Examples | `./build/debug/examples/...` | Demonstrate library features |
| CLI | `./build/release/apps/...` | Run terminal demos |
| Format | `./scripts/format_all.sh --check` | Check source formatting |
| Sanitizers | `AETHER_ENABLE_ASAN`, etc. | Runtime bug checks |
| Install smoke | `cmake --install ...` | Verify package/install output |
| Benchmarks | `./scripts/run_benchmarks.sh` | Run local benchmark suite |

Common CMake options used in this guide:

| Option | Purpose |
|---|---|
| `AETHER_BUILD_TESTS` | Build CTest test executables |
| `AETHER_BUILD_EXAMPLES` | Build runnable examples |
| `AETHER_BUILD_TOOLS` | Build manual tools such as `stress_spsc` |
| `AETHER_BUILD_APPS` | Build CLI apps under `apps/` |
| `AETHER_BUILD_BENCHMARKS` | Build optional benchmark executables |
| `AETHER_ENABLE_ASAN` | Enable AddressSanitizer |
| `AETHER_ENABLE_UBSAN` | Enable UndefinedBehaviorSanitizer |
| `AETHER_ENABLE_TSAN` | Enable ThreadSanitizer |
| `AETHER_ENABLE_CLANG_TIDY` | Enable clang-tidy integration when configured |
| `AETHER_ENABLE_INSTALL` | Enable install/export package rules |

## 1. Prerequisites

### macOS

```bash
xcode-select --install
brew install git cmake ninja llvm
export PATH="$(brew --prefix llvm)/bin:$PATH"
```

- `git` downloads the repository.
- `cmake` configures the build.
- `ninja` performs fast incremental builds.
- `llvm` provides `clang-format` and other LLVM tools used by local checks.

AppleClang is usually used by default on macOS unless you explicitly set `CC` and `CXX` to another compiler.

### Linux

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake ninja-build git
```

This installs a C++ compiler toolchain, CMake, Ninja, and Git on Debian/Ubuntu-style systems.

### Windows

The recommended Windows path is WSL2. Native Windows is not the main verified path because the project is POSIX/macOS/Linux-oriented for mmap behavior.

From PowerShell:

```powershell
wsl --install
```

After Ubuntu/WSL is installed, open the WSL terminal and follow the Linux commands above.

## 2. Clone the repository

```bash
cd ~/Downloads
git clone https://github.com/AyushmanRaha/Aether-Stream.git
cd Aether-Stream
git status
git branch
```

What each command does:

- `cd ~/Downloads` puts the project in your Downloads directory.
- `git clone` downloads the repository.
- `cd Aether-Stream` enters the project directory.
- `git status` should show a clean working tree.
- `git branch` should show `main` as the current branch.

To update an existing clone later:

```bash
cd ~/Downloads/Aether-Stream
git pull
```

## 3. Debug build and core tests

```bash
cmake -S . -B build/debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_BUILD_BENCHMARKS=OFF

cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
```

This block configures, builds, and tests the Debug tree.

- `-S .` tells CMake the source directory is the current directory.
- `-B build/debug` writes generated build files under `build/debug`.
- `-G Ninja` selects the Ninja generator.
- `Debug` is easier to inspect while developing.
- Tests, examples, tools, and apps are enabled.
- Benchmarks are off because they are optional and heavier.

Success means the build finishes without compiler errors and CTest ends with:

```text
100% tests passed, 0 tests failed out of 20
```

Helper alternative:

```bash
./scripts/run_tests.sh
```

That script configures Debug, builds, and runs tests for the standard local test path.

## 4. Run examples

```bash
./build/debug/examples/smoke
./build/debug/examples/basic_spsc
./build/debug/examples/mmap_smoke
./build/debug/examples/wal_replay
./build/debug/examples/broker_basic
./build/debug/examples/persistent_broker
```

Example purposes:

- `smoke`: confirms the built library reports its version.
- `basic_spsc`: shows SPSC queue publish/consume behavior.
- `mmap_smoke`: creates and tests mmap-backed file behavior.
- `wal_replay`: writes and replays sample WAL records.
- `broker_basic`: shows basic broker publish/consume behavior.
- `persistent_broker`: demonstrates WAL-backed broker behavior.

Expected output shape:

- `smoke` prints `Aether-Stream 0.1.0`.
- SPSC examples should show consumed values.
- WAL examples should show offsets, sequences, or payloads.
- Persistent broker output should show previously written records being replayed.

## 5. Run the manual stress tool

```bash
./build/debug/tools/stress_spsc
```

This exercises the SPSC queue with a larger message count. It is useful as a manual validation/stress check. A successful run should include:

```text
validation: passed
```

## 6. Build Release CLI apps

```bash
cmake -S . -B build/release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_BUILD_BENCHMARKS=OFF

cmake --build build/release
ctest --test-dir build/release --output-on-failure
```

Release is better for CLI demos and performance-sensitive local runs. Tests are still enabled so the Release configuration is verified before running demos.

## 7. Check CLI help

```bash
./build/release/apps/aether_bench --help
./build/release/apps/aether_pub --help
./build/release/apps/aether_sub --help
./build/release/apps/aether_replay --help
./build/release/apps/aether_inspect_wal --help
```

These commands print supported options. They do not need data files and are the safest first check for each CLI app.

## 8. CLI workflow: local broker benchmark demo

```bash
mkdir -p data
./build/release/apps/aether_bench --messages 100000 --payload-size 64 --capacity 1024
```

`aether_bench` runs a local two-thread broker demo.

- `--messages` controls the number of messages.
- `--payload-size` chooses the payload size.
- `--capacity` chooses queue capacity.
- Output includes metrics such as messages/sec, retry counters, and published/consumed counts.
- `metrics.published` and `metrics.consumed` should match the requested message count.
- `publish_failed_full` and `consume_failed_empty` are retry counters in a bounded queue demo; nonzero does not automatically mean failure.

This is not official benchmark evidence. For benchmark evidence, use `./scripts/run_benchmarks.sh` and preserve the raw output and environment context.

## 9. CLI workflow: write a WAL file

```bash
./build/release/apps/aether_pub --wal data/sample.wal --messages 1000
```

This writes generated demo `OrderEvent` records into a local WAL-backed broker. `data/sample.wal` is the local WAL file.

Success indicators include:

- `records written: 1000`
- `metrics.wal_records_written: 1000`
- `metrics.wal_failures: 0`
- `current WAL offset` advances

This does not publish over a network. It creates a local file for inspection and replay.

## 10. CLI workflow: inspect the WAL

```bash
./build/release/apps/aether_inspect_wal --wal data/sample.wal
./build/release/apps/aether_inspect_wal --wal data/sample.wal --verbose --limit 5
```

The first command summarizes the WAL. The second command prints details for up to five records.

Important fields include:

- `record count`
- `total payload bytes`
- `first sequence`
- `last sequence`
- `final reader offset`
- `scan ended cleanly`
- `metrics.recovered_records`
- `metrics.recovery_failures`

Success indicators include:

- `scan ended cleanly: yes`
- `metrics.recovery_failures: 0`

This tool reports and summarizes WAL contents. It does not repair WAL files.

## 11. CLI workflow: raw replay

```bash
./build/release/apps/aether_replay --wal data/sample.wal --limit 10
```

This reads raw WAL records and prints offset, sequence, timestamp, payload size, flags, checksum, and preview. The `preview` field is raw printable payload bytes, not a typed decoded object.

Success indicators include:

- `summary: records replayed=10`
- `metrics.recovery_failures: 0`

## 12. CLI workflow: subscriber demo with WAL

```bash
./build/release/apps/aether_sub --wal data/sample.wal --limit 10
```

This is a local demo subscriber. With `--wal`, it replays typed demo `OrderEvent` records written by `aether_pub`. It should print the first 10 order-like records and summarize the rest.

Success indicators include:

- `summary: replayed=1000 printed=10 suppressed=990`
- `metrics.recovered_records: 1000`
- `metrics.recovery_failures: 0`

This is not a live network subscriber. It is replaying a local file.

## 13. CLI workflow: subscriber demo without WAL

```bash
./build/release/apps/aether_sub --messages 25 --capacity 1024 --limit 10
```

Without `--wal`, this creates an in-process local broker demo. It publishes generated values and consumes them immediately.

Success indicators include:

- `summary: produced=25 consumed=25 suppressed=15`
- `metrics.published: 25`
- `metrics.consumed: 25`
- failure counters ideally remain `0` for this small case

## 14. Full CLI sequence

After the Release build exists, this block runs the complete CLI demo flow:

```bash
mkdir -p data

./build/release/apps/aether_bench --messages 100000 --payload-size 64 --capacity 1024

./build/release/apps/aether_pub --wal data/sample.wal --messages 1000

./build/release/apps/aether_inspect_wal --wal data/sample.wal
./build/release/apps/aether_inspect_wal --wal data/sample.wal --verbose --limit 5

./build/release/apps/aether_replay --wal data/sample.wal --limit 10

./build/release/apps/aether_sub --wal data/sample.wal --limit 10
./build/release/apps/aether_sub --messages 25 --capacity 1024 --limit 10
```

## 15. Formatting check

```bash
./scripts/format_all.sh --check
```

This checks C/C++ source formatting using `clang-format`. A successful run ends with `clang-format completed`.

On macOS, install LLVM via Homebrew if `clang-format` is missing:

```bash
brew install llvm
export PATH="$(brew --prefix llvm)/bin:$PATH"
```

## 16. Sanitizer builds

### AddressSanitizer + UndefinedBehaviorSanitizer

```bash
cmake -S . -B build/asan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_ENABLE_ASAN=ON \
  -DAETHER_ENABLE_UBSAN=ON

cmake --build build/asan
ctest --test-dir build/asan --output-on-failure
```

ASAN helps catch memory errors. UBSAN helps catch undefined behavior. Success is again all CTest tests passing.

### ThreadSanitizer

```bash
cmake -S . -B build/tsan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_ENABLE_TSAN=ON

cmake --build build/tsan
ctest --test-dir build/tsan --output-on-failure
```

TSAN helps detect data races. It can be slower than normal tests. Passing TSAN is a strong local signal for concurrency-oriented code, but it is not a production-readiness claim.

## 17. Package/install smoke test

```bash
cmake -S . -B build/package -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_ENABLE_INSTALL=ON

cmake --build build/package
ctest --test-dir build/package --output-on-failure
cmake --install build/package --prefix install/aether

find install/aether -maxdepth 4 -type f | sort
```

This verifies the install/export rules. It installs into local `install/aether`, not system-wide.

Expected outputs include:

- `lib/libaether_stream.a`
- public headers under `include/aether/`
- CMake package files under `lib/cmake/AetherStream/`

## 18. Benchmarks

```bash
./scripts/run_benchmarks.sh
```

This script:

- configures a Release build with `AETHER_BUILD_BENCHMARKS=ON`
- builds benchmarks
- runs tests before benchmarks
- writes environment data
- runs `bench_spsc_throughput`
- runs `bench_spsc_latency`
- runs `bench_payload_sizes`
- runs `bench_broker_end_to_end`
- runs `bench_batch_publish`
- runs `bench_zero_copy_spsc`
- runs `bench_spin_wait`
- writes raw outputs under `benchmark-results/<timestamp>/`

Benchmark numbers are local synthetic measurements. They depend on machine, OS, thermal state, load, compiler, and build type. Do not compare them as production guarantees. Preserve raw outputs and environment context before publishing numbers.

Common macOS benchmark warnings include:

```text
Unable to determine clock rate from sysctl: hw.cpufrequency: No such file or directory
This does not affect benchmark measurements, only the metadata output.
***WARNING*** Failed to set thread affinity. Estimated CPU frequency may be incorrect.
```

On macOS/Apple Silicon, CPU frequency and thread affinity metadata may be limited. These warnings do not necessarily mean the benchmark failed. The important success signal is that all benchmark executables complete and raw outputs are written.

## 19. One-shot full local verification

Beginners should read the earlier sections before running this. The block below intentionally does a lot: it installs prerequisites where reasonable, clones or updates the repo, removes generated outputs, runs builds, tests, examples, CLI demos, formatting, sanitizers, install smoke, and benchmarks.

```bash
set -e

cd ~/Downloads

if command -v apt-get >/dev/null 2>&1; then
  sudo apt-get update
  sudo apt-get install -y build-essential cmake ninja-build git
elif command -v brew >/dev/null 2>&1; then
  brew install git cmake ninja llvm
  export PATH="$(brew --prefix llvm)/bin:$PATH"
fi

if [ -d "Aether-Stream/.git" ]; then
  cd Aether-Stream
  git pull
else
  git clone https://github.com/AyushmanRaha/Aether-Stream.git
  cd Aether-Stream
fi

rm -rf build install data benchmark-results
mkdir -p data

cmake -S . -B build/debug -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_BUILD_BENCHMARKS=OFF
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
./build/debug/examples/smoke
./build/debug/examples/basic_spsc
./build/debug/examples/mmap_smoke
./build/debug/examples/wal_replay
./build/debug/examples/broker_basic
./build/debug/examples/persistent_broker
./build/debug/tools/stress_spsc

cmake -S . -B build/release -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_BUILD_BENCHMARKS=OFF
cmake --build build/release
ctest --test-dir build/release --output-on-failure
./build/release/apps/aether_bench --help
./build/release/apps/aether_pub --help
./build/release/apps/aether_sub --help
./build/release/apps/aether_replay --help
./build/release/apps/aether_inspect_wal --help
./build/release/apps/aether_bench --messages 100000 --payload-size 64 --capacity 1024
./build/release/apps/aether_pub --wal data/sample.wal --messages 1000
./build/release/apps/aether_inspect_wal --wal data/sample.wal
./build/release/apps/aether_inspect_wal --wal data/sample.wal --verbose --limit 5
./build/release/apps/aether_replay --wal data/sample.wal --limit 10
./build/release/apps/aether_sub --wal data/sample.wal --limit 10
./build/release/apps/aether_sub --messages 25 --capacity 1024 --limit 10

./scripts/format_all.sh --check

cmake -S . -B build/asan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_ENABLE_ASAN=ON \
  -DAETHER_ENABLE_UBSAN=ON
cmake --build build/asan
ctest --test-dir build/asan --output-on-failure

cmake -S . -B build/tsan -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON \
  -DAETHER_ENABLE_TSAN=ON
cmake --build build/tsan
ctest --test-dir build/tsan --output-on-failure

cmake -S . -B build/package -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_ENABLE_INSTALL=ON
cmake --build build/package
ctest --test-dir build/package --output-on-failure
cmake --install build/package --prefix install/aether
find install/aether -maxdepth 4 -type f | sort

./scripts/run_benchmarks.sh

echo "Aether-Stream full local verification completed successfully."
```

## 20. Clean rebuild

```bash
cd ~/Downloads/Aether-Stream
rm -rf build install data benchmark-results
```

This removes generated build, install, demo, and benchmark outputs. It does not delete source code or Git history.

## 21. Troubleshooting

| Symptom | Meaning | Fix |
|---|---|---|
| `cmake: command not found` | CMake missing | Install CMake |
| `ninja: command not found` | Ninja missing | Install Ninja |
| `clang-format not found` | LLVM tools not on PATH | Install LLVM and export PATH |
| CTest failure | A test failed | Re-run with `--output-on-failure` and inspect failing test |
| WAL file missing | `aether_pub` not run yet | Run `aether_pub --wal data/sample.wal --messages 1000` |
| macOS benchmark CPU warning | Metadata limitation | Usually safe if benchmarks complete |
| Thread affinity warning | Platform limitation | Usually safe on macOS |
| Permission denied on script | Script not executable | Run `chmod +x scripts/*.sh` |

## 22. Final success checklist

- [ ] Repository cloned or updated
- [ ] Debug build completed
- [ ] Debug tests passed
- [ ] Examples ran
- [ ] Stress tool passed
- [ ] Release build completed
- [ ] Release tests passed
- [ ] CLI help printed
- [ ] WAL created, inspected, and replayed
- [ ] Local subscriber mode worked
- [ ] Formatting check passed
- [ ] ASAN/UBSAN tests passed
- [ ] TSAN tests passed
- [ ] Package/install smoke test passed
- [ ] Benchmarks completed and wrote raw outputs

Passing all of these means the local project is functioning correctly on that machine. It still does not imply production readiness or distributed broker behavior.
