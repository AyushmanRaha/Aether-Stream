# Low-Latency Design Notes

These notes describe design tradeoffs for latency-oriented local primitives. They are not production-readiness or latency-guarantee claims.

## SPSC scope

The queue deliberately supports exactly one producer and one consumer. This keeps ownership and memory ordering reviewable: one thread advances `head`, one thread advances `tail`, and acquire/release operations publish object lifetime changes.

## Batching

Batch APIs reduce repeated broker call overhead for local in-memory flows. Persistent batching is not added because WAL ordering, partial append handling, and recovery semantics require a separate design.

## Zero-copy reservations

`ZeroCopySpsc` exposes reservation/commit/cancel semantics so payloads can be constructed in-place. It remains experimental and SPSC-only.

## Spin waiting

`SpinWait` and `cpu_relax` are small utilities for short local wait loops. They must be used carefully because busy waiting can waste CPU and distort measurements.

## CPU affinity

CPU affinity helpers are Linux-first and report unsupported behavior on platforms where equivalent behavior is not implemented. Affinity is not a guarantee of stable latency.

## WAL tradeoff

WAL-before-queue publication provides a simple local ordering rule: the persistent broker only publishes after append succeeds. That ordering can add latency, especially when flushing is enabled.

## Missing production concerns

This repository does not implement network transport, kernel bypass, service operations, replication, failover, durable recovery tooling, or production observability. Benchmark results must be treated as local measurements with environment context.
