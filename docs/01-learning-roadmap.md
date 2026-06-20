# Learning Roadmap

This checklist is for students and contributors who want to understand the project while it grows from a clean baseline into a systems-level C++ message broker.

## Phase 0 foundation

Start with the repository basics:

- Git basics: clone, branch, commit, diff, and status.
- Reading repo layouts: understand why docs, scripts, source, tests, and build output are separated.
- CMake basics: configure step, build step, generators, build directories, and targets.
- Formatting and linting purpose: keep code review focused on behavior instead of whitespace.
- Shell scripts: read simple Bash scripts, understand `set -euo pipefail`, and run local checks safely.

## C++ foundations

Before implementing the library, become comfortable with:

- fixed-width integers such as `std::uint64_t` and `std::uint32_t`;
- RAII for resource ownership and cleanup;
- templates for reusable type-safe components;
- move semantics for efficient ownership transfer;
- `std::span` for non-owning views over contiguous memory;
- `std::byte` for raw binary data.

## Concurrency

The lock-free portions will require careful study of:

- `std::thread` for basic concurrent execution;
- `std::atomic` for shared state without data races;
- `memory_order_acquire` and `memory_order_release`;
- data races and why undefined behavior matters;
- false sharing between CPU cores;
- cache lines and cache-friendly data layout.

## Systems programming

Persistence and low-level I/O will require:

- file descriptors;
- `mmap`;
- `msync`;
- binary file formats;
- checksums.

## Testing and benchmarking

Correctness and performance work should be learned separately:

- unit tests for small deterministic behavior;
- stress tests for concurrency and edge cases;
- sanitizers such as AddressSanitizer, UndefinedBehaviorSanitizer, and ThreadSanitizer;
- Google Benchmark basics;
- throughput versus latency;
- percentile latency metrics such as p50, p95, and p99.

## Suggested learning order by project phase

0. Learn Git, repository layout, CMake basics, formatting files, and safe shell scripts.
1. Learn CMake targets, include paths, compile features, and library organization.
2. Learn fixed-width integer types, message schemas, ownership, and binary compatibility.
3. Learn circular buffers, atomics, acquire/release ordering, and SPSC constraints.
4. Learn data-race prevention, stress testing, sanitizers, false sharing, and cache alignment.
5. Learn benchmark design, measurement noise, throughput, latency, p50, p95, and p99.
6. Learn file descriptors, `mmap`, page sizes, flushing, and platform differences.
7. Learn WAL record formats, checksums, recovery, truncation, and corruption handling.
8. Learn API design for producers, consumers, broker lifecycle, and backpressure.
9. Learn CLI design, examples, argument parsing, and user-facing workflows.
10. Learn metrics, diagnostics, structured logging, and observability tradeoffs.
11. Learn CI, sanitizer jobs, packaging basics, and reproducible developer environments.
12. Learn advanced low-latency topics such as batching, NUMA awareness, and CPU affinity.
13. Learn documentation polish, portfolio presentation, design writeups, and release notes.
