# Phase 12 Low-Latency Tuning Notes

These are Phase 12 tuning notes for the experimental low-latency APIs. They are not Phase 13 portfolio documentation and they do not contain official performance claims.

## Batching

Batching amortizes API checks, metrics updates, and loop overhead across several messages. That can improve throughput when the producer already has multiple messages ready. The tradeoff is latency: waiting to form a larger batch can hurt tail latency, especially when traffic is sparse or bursty.

`aether::BatchBroker<T, Capacity>` is intentionally in-memory only in Phase 12. Persistent batching is not included yet because WAL ordering, partial append handling, and recovery semantics should be designed separately.

## Zero-copy reservation

`aether::ZeroCopySpsc<T, Capacity>` lets the producer reserve a slot, construct directly in that slot, and release-publish it with `commit()`. This avoids an extra producer-side copy or move into queue storage. It still requires clear ownership: uncommitted data is never visible to the consumer, `cancel()` destroys constructed-but-uncommitted data, and reservations must not outlive the queue.

## Spin waiting

`aether::utils::cpu_relax()` is a short CPU hint for tight polling. `std::this_thread::yield()` hands execution back to the scheduler and may reduce CPU burn at the cost of latency variance. `aether::utils::SpinWait` combines early relax loops with optional yielding. Sleeping is usually too slow for low-latency hot paths, but spinning can waste CPU and should be benchmarked on the deployment machine.

## CPU affinity

Phase 12 provides Linux-first current-thread pinning through `pthread_setaffinity_np`. On macOS there is no equivalent Linux-style portable CPU affinity API in this project, so the implementation reports unsupported and the pin/clear calls are safe no-op fallbacks for valid-looking indexes. Laptops, turbo states, thermal throttling, and power-management policy can dominate results even when affinity is available.

## Cache lines and false sharing

The queues use the existing `CachePadded` utility to separate hot producer and consumer counters. This reduces false sharing pressure, but it is not a substitute for measuring cache behavior under realistic workloads.

## Benchmarking honesty

Use Release builds for any local numbers and preserve the raw commands and output. Benchmark smoke jobs only catch regressions in benchmark executables; they are not official results. `scripts/run_benchmarks.sh` runs the Phase 12 `bench_batch_publish`, `bench_zero_copy_spsc`, and `bench_spin_wait` executables along with the earlier benchmarks. Do not claim HFT-grade latency from macOS laptop runs or from synthetic microbenchmarks.
