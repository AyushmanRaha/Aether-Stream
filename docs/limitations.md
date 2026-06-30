# Aether-Stream Limitations

## Current status

Aether-Stream is complete through Phase 13 as a local C++20 systems project and portfolio-ready documentation package. It is not production-ready and does not claim networking, distributed broker behavior, or official low-latency results without raw benchmark evidence.

## Concurrency limitations

- The core queue is SPSC only: exactly one producer thread and exactly one consumer thread.
- There is no MPSC or MPMC queue implementation.
- Queue operations are non-blocking attempts; there are no blocking wait APIs.
- `size_approx()` is observational and must not be treated as synchronization.

## Persistence limitations

- WAL files are fixed-size mmap-backed files.
- There is no WAL rotation.
- There is no multi-segment WAL implementation.
- There is no production recovery, repair, truncation, or compaction tooling.
- There is no schema evolution.
- Typed replay is same-program/same-platform only and requires trivially copyable payloads.

## Deployment/product limitations

- No networking.
- No live cross-process broker service.
- No distributed broker semantics.
- No authentication, TLS, ACLs, service discovery, replication, or clustering.

## Benchmark limitations

- Reportable results require Release builds.
- macOS and laptop results are development numbers.
- Final low-latency claims require controlled Linux benchmarking with pinned cores and a quiet system.
- Benchmark smoke checks prove executables run; they are not official results.
- Manual stress-tool output is not benchmark output.

## Experimental API limitations

- `ZeroCopySpsc` is experimental.
- CPU affinity is Linux-first; macOS and unsupported platforms use fallback/no-op behavior.
- Spin waits are workload-, scheduler-, and hardware-sensitive.

## What would be needed for production

- MPSC/MPMC or a sharded SPSC design.
- Bounded backpressure policies and blocking/wait strategies.
- Durable flush/fsync policy appropriate to the product requirements.
- WAL rotation, recovery, truncation, and repair tooling.
- IPC or network layer with clear protocol semantics.
- Observability exporters, tracing, and operational metrics.
- Long-running soak tests, fault injection, and crash-recovery validation.
