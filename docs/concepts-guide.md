# Concepts Guide

This guide explains the technical concepts used by Aether-Stream without presenting them as a build-history checklist.

## C++20 foundations

The library uses C++20, CMake targets, namespace-scoped public headers, RAII resource management, typed status values, and an expected-like result wrapper. Public consumers link the exported alias target `aether::stream`. In short, the codebase favors modern, type-safe C++ patterns — resources clean themselves up automatically, and normal recoverable errors are returned as values instead of thrown as exceptions.

## SPSC concurrency

The core queue is single-producer/single-consumer only. That contract lets the producer own the head counter and the consumer own the tail counter while using acquire/release operations for cross-thread publication and slot reuse. In practical terms, this is a single conveyor belt: exactly one thread places items on one end, and exactly one thread removes them from the other.

## Memory ordering

The producer constructs a payload, then release-stores the updated head. The consumer acquire-loads the head before reading the slot, destroys the payload after consuming it, and release-stores the updated tail. The producer acquire-loads the tail before reusing a slot. In practical terms, the producer and consumer hand off ownership of each queue slot one step at a time, and the acquire/release operations are what guarantee each side sees the other's work in the right order.

## mmap and WAL persistence

`MmapFile` wraps POSIX-oriented file mapping. The WAL writer serializes fixed-format records with CRC32 validation, and the reader scans records sequentially. Persistent broker publishing appends to the WAL before publishing to the in-process queue. In practical terms, the WAL is a journal file: every message is appended as one entry before it is delivered, so the journal can later be replayed to reconstruct what happened.

## Broker APIs

`Broker` wraps the SPSC queue for local publish/consume flows. `BatchBroker` adds batch-oriented local APIs. `PersistentBroker` combines WAL append with queue publication for trivially copyable payloads. In practical terms, a broker is the local mailbox in front of the queue: code calls a publish method to drop a value in and a consume method to take one out.

## Metrics

Broker counters and snapshots expose local diagnostic information. The latency histogram supports local measurement summaries without becoming an external observability system. In practical terms, these are simple counters and timers for understanding what the library is doing locally, not a replacement for production monitoring infrastructure.

## Benchmarks

Benchmark executables exist for queue, broker, batch, zero-copy, payload-size, and spin-wait paths. Publishable numbers must come from `./scripts/run_benchmarks.sh` and preserve measured output plus environment metadata as sanitized raw files or consolidated redacted Markdown evidence. In practical terms, a benchmark here is a small, repeatable local program that measures one specific operation, and only numbers produced this way are reported as results.

## Build and verification

CMake options enable tests, examples, tools, apps, benchmarks, sanitizers, clang-tidy, and install/export checks. CTest covers core behavior; benchmark smoke checks validate that benchmark executables build and run briefly, not that any performance claim is established. In practical terms, these are the automated checks that must pass before code is trusted: does it build, do the tests pass, and do the stricter runtime checks (sanitizers) find anything wrong.
