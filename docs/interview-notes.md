# Aether-Stream Interview Notes

## 30-second pitch

Aether-Stream is a C++20 systems project that builds a local lock-free SPSC messaging stack: a queue, broker APIs, mmap-backed WAL persistence, metrics, CLI demos, benchmarks, CI, and honest documentation. It demonstrates memory ordering, object lifetime, binary formats, benchmark discipline, and clear non-goals.

## 2-minute explanation

The core is `SpscRingBuffer<T, Capacity>`, designed for exactly one producer and one consumer. The producer owns `head_`, the consumer owns `tail_`, and acquire/release atomics publish constructed slots and reusable slots. On top of that, the broker APIs provide local publish/consume semantics, while the persistent broker appends serialized trivially copyable events to a WAL before publishing to the queue. The WAL uses an explicit little-endian v1 header and CRC32 validation. CLI apps and benchmarks make the project runnable, while CI, sanitizers, clang-tidy, and package install checks make it maintainable.

## 5-minute explanation

Start with scope: this is a local C++20 toolkit, not a distributed broker. Then explain the layers: core types/status/configs, SPSC queue, broker wrappers, WAL/mmap persistence, metrics, CLI apps, benchmarks, and CI. The interesting technical work is in carefully bounding concurrency to SPSC, using acquire/release instead of global sequential consistency, managing raw slot lifetime without default-constructing the whole buffer, avoiding false sharing, validating WAL records, and refusing to publish benchmark claims without raw outputs.

## 15-minute deep dive outline

1. Scope and non-goals.
2. SPSC ring-buffer counters, masking, full/empty distinction, and slot lifecycle.
3. Acquire/release memory-order protocol and why it is SPSC-only.
4. Broker composition over the queue.
5. WAL-before-queue semantics and typed replay limitations.
6. mmap RAII layer and fixed-size file model.
7. Metrics counters, snapshots, and latency histogram.
8. CLI demo flow.
9. Benchmark runner, raw outputs, and no-fake-results policy.
10. CI, sanitizers, clang-tidy, and package install verification.
11. Honest limitations and credible next steps.

## What I built

- C++20 core library.
- Lock-free SPSC queue.
- mmap layer.
- WAL writer/reader.
- In-memory and persistent broker APIs.
- Metrics counters, snapshots, and latency histogram.
- CLI tools: `aether_bench`, `aether_pub`, `aether_sub`, `aether_replay`, `aether_inspect_wal`.
- Google Benchmark suite.
- CI, sanitizers, clang-tidy, format, benchmark smoke, and package verification.
- Phase 12 low-latency APIs: `BatchBroker`, experimental `ZeroCopySpsc`, `SpinWait`, `cpu_relax`, CPU affinity helper.

## Hardest engineering problems

- Memory ordering for publishing data without overusing `seq_cst`.
- Object lifetime in raw storage slots.
- Avoiding false sharing between hot producer and consumer counters.
- WAL record format, checksum calculation, and corruption validation.
- Benchmark honesty and reproducibility.
- Keeping docs honest about what is and is not implemented.

## Tradeoffs I can defend

- SPSC instead of MPMC: simpler, faster, and easier to reason about correctly for one producer and one consumer.
- Acquire/release instead of `seq_cst` everywhere: enough ordering for publication without unnecessary global ordering.
- mmap abstraction before WAL: isolates file mapping and resource lifetime from record-format logic.
- WAL-before-queue semantics: if publish succeeds, the record append succeeded first.
- No fake networking: a local CLI demo is not a broker service.
- Benchmarks separated from tests: correctness tests should be stable; performance experiments need raw outputs and environment metadata.

## Likely interviewer questions and answers

### Why lock-free?

The SPSC path avoids mutex handoff overhead for a bounded one-producer/one-consumer workload. The project uses lock-free behavior where the scope is narrow enough to validate.

### Why SPSC only?

SPSC lets each side own one counter and use a small acquire/release protocol. MPSC/MPMC would require different algorithms, contention management, and correctness proofs.

### What memory orders are used?

The owner loads its own counter relaxed. Cross-thread visibility uses acquire loads and release stores: producer release-stores `head_` after constructing a slot; consumer acquire-loads `head_` before reading it; consumer release-stores `tail_` after destroying a slot; producer acquire-loads `tail_` before reusing storage.

### How does the ring buffer distinguish full from empty?

It uses monotonic logical counters rather than only physical indexes. Empty is `head == tail`; full is `head - tail >= Capacity`; physical slot index is `counter & (Capacity - 1)`.

### What happens if the WAL is corrupted?

The reader stops with `StatusCode::corrupted_record` for invalid magic, version, header size, or checksum mismatch. Zero-filled or incomplete tails are treated as clean EOF/stop. There is no repair or truncation tooling.

### Why not claim HFT-ready?

The project has HFT-style primitives and design notes, but HFT readiness requires controlled hardware, kernel tuning, networking, fault tolerance, operations, and verified latency distributions. This repository does not claim those.

### What would you improve next?

Potential next work would be production-oriented recovery tooling, a sharded or MPSC/MPMC design, richer backpressure, controlled Linux benchmark publication, and optional IPC/network layers with clear semantics.

### How would you add MPSC?

Do not bolt it onto the SPSC queue. Use a separate algorithm such as a bounded MPSC ring with atomic producer reservation or shard producers across SPSC queues and merge downstream.

### How would you make persistence production-ready?

Define fsync policy, segment rotation, checkpoints, truncation/repair tools, schema evolution, crash tests, fault injection, and operational recovery procedures.

### How do you validate correctness?

Use focused unit-style CTest executables, concurrent ordered-transfer tests, stress tests across capacities, move-only payload tests, WAL corruption tests, sanitizers, clang-tidy, and CI builds across platforms.
