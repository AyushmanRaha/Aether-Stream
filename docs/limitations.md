# Limitations

Aether-Stream is a local C++20 systems project. It is not production-ready and does not claim networking, distributed broker behavior, or official low-latency results without raw benchmark evidence.

## Messaging model

- SPSC only: exactly one producer and one consumer for the queue contracts.
- No MPSC or MPMC queue support.
- No blocking wait API.
- No network transport, daemon, cluster, service discovery, TLS, auth, replication, or high availability.
- No live cross-process broker subscription model.

## Persistence model

- WAL files are fixed-size local mapped files.
- No WAL rotation, truncation, repair, compaction, schema evolution, or production recovery tooling.
- Typed replay is same-program/same-platform and requires trivially copyable payloads.
- Flush behavior depends on configuration and platform storage semantics.

## Platform model

- The mmap implementation is POSIX-oriented.
- Non-POSIX paths report unsupported behavior rather than providing native mapping semantics.
- CPU affinity helpers are platform-dependent and Linux-first.
- Windows users should prefer WSL2 unless native Windows support is explicitly verified.

## Benchmark model

- No official benchmark numbers are published yet.
- Benchmark smoke checks are not performance results.
- Stress tools are correctness aids, not benchmark results.
- Published numbers must come from `./scripts/run_benchmarks.sh` with environment metadata and measured output preserved as sanitized raw files or consolidated redacted Markdown evidence.
