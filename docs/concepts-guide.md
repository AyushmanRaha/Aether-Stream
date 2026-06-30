# Concepts Guide

This guide explains the technical concepts used by Aether-Stream without presenting them as a build-history checklist.

## C++20 foundations

The library uses C++20, CMake targets, namespace-scoped public headers, RAII resource management, typed status values, and an expected-like result wrapper. Public consumers link the exported alias target `aether::stream`.

## SPSC concurrency

The core queue is single-producer/single-consumer only. That contract lets the producer own the head counter and the consumer own the tail counter while using acquire/release operations for cross-thread publication and slot reuse.

## Memory ordering

The producer constructs a payload, then release-stores the updated head. The consumer acquire-loads the head before reading the slot, destroys the payload after consuming it, and release-stores the updated tail. The producer acquire-loads the tail before reusing a slot.

## mmap and WAL persistence

`MmapFile` wraps POSIX-oriented file mapping. The WAL writer serializes fixed-format records with CRC32 validation, and the reader scans records sequentially. Persistent broker publishing appends to the WAL before publishing to the in-process queue.

## Broker APIs

`Broker` wraps the SPSC queue for local publish/consume flows. `BatchBroker` adds batch-oriented local APIs. `PersistentBroker` combines WAL append with queue publication for trivially copyable payloads.

## Metrics

Broker counters and snapshots expose local diagnostic information. The latency histogram supports local measurement summaries without becoming an external observability system.

## Benchmarks

Benchmark executables exist for queue, broker, batch, zero-copy, payload-size, and spin-wait paths. Publishable numbers must come from `./scripts/run_benchmarks.sh` and preserve raw output plus environment metadata.

## Build and verification

CMake options enable tests, examples, tools, apps, benchmarks, sanitizers, clang-tidy, and install/export checks. CTest covers core behavior; benchmark smoke checks validate that benchmark executables build and run briefly, not that any performance claim is established.
