# Phase 12 HFT-Style Design Notes

These notes describe the Phase 12 low-latency design choices. They are not production or HFT readiness claims.

## SPSC scope

Aether-Stream focuses on single-producer/single-consumer queues because SPSC ownership gives a small, reviewable synchronization protocol: one producer advances `head`, one consumer advances `tail`, and acquire/release ordering publishes object lifetime changes. MPMC queues are not added in Phase 12 because they require a different contention, reclamation, and fairness design.

## Batch versus single-message APIs

Single-message APIs minimize per-message waiting and are easiest to reason about for latency. Batch APIs amortize checks and loop overhead and can improve throughput when bursts are already available. Large batches can hurt tail latency if messages wait for a batch to fill.

## Zero-copy reservation correctness

The reservation/commit protocol constructs directly in queue storage and only advances `head` on `commit()`. `cancel()` and the reservation destructor clean up uncommitted objects. Correctness risks include forgetting to commit, constructing twice into a slot, or destroying a queue while a reservation is still alive; the API documents those constraints rather than hiding them behind allocations or locks.

## CPU pinning and scheduler effects

Linux affinity can reduce migration noise by pinning the current thread to a hardware thread. It does not disable interrupts, frequency scaling, SMT sibling contention, or scheduler effects elsewhere in the system. macOS behavior is a safe no-op fallback in this project.

## WAL durability and latency

The persistent broker writes to the WAL before queue publication to preserve a simple durability story. That can add latency, especially when flushing. Phase 12 batch and zero-copy APIs deliberately do not rewrite WAL semantics.

## What real HFT systems still require

Real trading systems would still need kernel bypass or specialized networking, NIC timestamping, NUMA-aware deployment, careful Linux tuning, production-grade monitoring, and rigorous replay/audit guarantees. This repository remains an educational systems-programming project.

## Interview talking points

- Correctness comes before benchmarking.
- Acquire/release memory ordering publishes slot lifetimes without locks in SPSC ownership.
- Cache-line padding reduces false sharing on hot counters.
- Benchmarks such as `bench_batch_publish`, `bench_zero_copy_spsc`, and `bench_spin_wait` must be run in Release mode and reported as raw/local unless a controlled measurement process exists.
- Avoid unsupported performance claims.
