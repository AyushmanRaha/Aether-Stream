# Ring Buffer Design

## Current status

This document describes the Phase 3-4 single-producer/single-consumer (SPSC) queue currently implemented in `include/aether/spsc_ring_buffer.hpp`.

Phase 5 has since added benchmark infrastructure for measuring this queue, but this file remains the design document for the queue algorithm. See `docs/benchmark-methodology.md` and `docs/performance-results.md` for benchmark process and result reporting.

Aether-Stream now has a Phase 8 broker layer, but this document remains focused only on the SPSC queue algorithm. The higher-level broker API is documented in `docs/broker-api.md`. The ring buffer is a reusable component; the queue itself does not own persistence, networking, or WAL logic.

## Scope and non-goals

Supported today:

- exactly one producer thread calling `try_push()` or `try_emplace()`;
- exactly one consumer thread calling `try_pop()`;
- a compile-time fixed-capacity, power-of-two queue;
- non-blocking push/pop attempts that report success or failure with `bool`;
- copyable, movable, move-only, and non-default-constructible payload types when they satisfy the relevant operation constraints.

Unsupported today:

- MPSC queues with multiple producer threads;
- MPMC queues with multiple producers and multiple consumers;
- blocking wait APIs;
- cross-process queues;
- persistence, WAL storage, network transport, or direct mmap integration inside the queue itself;
- production-ready, HFT-ready, or unsupported performance claims;
- official committed latency/throughput result tables without raw benchmark output.

The queue itself does not perform persistence, WAL storage, network transport, or direct mmap integration. Phase 8 composes the queue with WAL at the broker layer; see `docs/broker-api.md`.

The manual stress tool is for correctness and stress validation only. It should not be presented as a benchmark result. For performance experiments, use the Phase 5 Google Benchmark executables and preserve raw output from `scripts/run_benchmarks.sh`.

## Public API overview

The queue is a header-only template named `aether::SpscRingBuffer<T, Capacity>`. `Capacity` is a compile-time template argument, must be a power of two, and must be at least `2`.

```cpp
#include <aether/spsc_ring_buffer.hpp>

#include <iostream>

int main() {
    aether::SpscRingBuffer<int, 1024> queue;

    if (!queue.try_push(42)) {
        std::cerr << "queue is full\n";
        return 1;
    }

    queue.try_emplace(43);

    int value = 0;
    while (queue.try_pop(value)) {
        std::cout << value << '\n';
    }

    const bool currently_empty = queue.empty();
    const bool currently_full = queue.full();
    const std::size_t max_items = queue.capacity();
    const std::size_t observed_size = queue.size_approx();

    (void)currently_empty;
    (void)currently_full;
    (void)max_items;
    (void)observed_size;
}
```

Current operations:

- `try_push(const T&)`: copy-constructs into the queue when `T` is copy-constructible.
- `try_push(T&&)`: move-constructs into the queue when `T` is move-constructible.
- `try_emplace(args...)`: constructs `T` directly in the selected slot.
- `try_pop(T& out)`: move-assigns the front item into `out` and destroys the slot object.
- `empty()`: returns an observational empty check.
- `full()`: returns an observational full check.
- `capacity()`: returns the compile-time capacity.
- `size_approx()`: returns an approximate size observation, especially under concurrent producer/consumer activity.

## Core design

The queue stores elements in a fixed array of raw slots:

- logical producer position: `head_`;
- logical consumer position: `tail_`;
- physical slot array: `buffer_[Capacity]`;
- physical index mask: `mask_ = Capacity - 1`.

`head_` and `tail_` are monotonic logical counters. They are not physical indexes that wrap after reaching `Capacity`. The producer publishes progress by advancing `head_`; the consumer publishes progress by advancing `tail_`.

A physical slot is selected with:

```cpp
physical_index = logical_index & (Capacity - 1);
```

This works because `Capacity` is required to be a power of two. For power-of-two capacities, `index & mask_` is an efficient modulo operation equivalent to `index % Capacity` for selecting a slot.

Unlike the common “one empty slot” circular-buffer design, this implementation can use all `Capacity` slots. It can distinguish full from empty because `head_` and `tail_` are logical counters:

- empty means `head == tail`;
- full means `head - tail >= Capacity`.

The physical slot index may wrap many times, but the logical distance between `head_` and `tail_` tells the queue how many live objects are currently present.

A small mental model:

```text
logical counters:  ... tail ........ head ...
occupied distance:       head - tail
physical slot:      logical_index & (Capacity - 1)
```

## Slot lifecycle

The queue does not default-construct `Capacity` objects up front. Instead, each slot is raw storage:

- `std::construct_at(...)` creates a `T` object in a slot when a push/emplace succeeds;
- `std::launder(reinterpret_cast<T*>(...))` is used to obtain a valid pointer to the object in raw storage;
- `std::destroy_at(...)` destroys the object after it has been consumed;
- the destructor walks from `tail_` to `head_` and destroys any objects that remain live.

The normal lifecycle is:

1. Producer observes that space is available.
2. Producer constructs the object in the selected raw slot.
3. Producer release-publishes the new `head_` value.
4. Consumer acquire-observes `head_` and sees the object as available.
5. Consumer moves the object into `out`.
6. Consumer destroys the object in the slot.
7. Consumer release-publishes the new `tail_` value, making the slot available for reuse.

This explicit object-lifetime management matters because the queue supports payloads that are move-only or not default-constructible. The tests in `tests/test_spsc_move_only.cpp` cover `std::unique_ptr<int>` and a custom move-only type.

Push/pop operations do not allocate memory dynamically unless the payload type `T` itself allocates internally. There are no mutexes in the queue hot path.

## Full and empty behavior

The empty condition is:

```cpp
head == tail
```

The full condition is:

```cpp
head - tail >= Capacity
```

When the queue is full, `try_push()` and `try_emplace()` return `false` instead of blocking. This is the queue's backpressure signal to the caller.

When the queue is empty, `try_pop()` returns `false` instead of blocking. Callers that want to wait can add their own policy outside the queue, such as a spin/yield loop using `aether::utils::SpinWait` from `include/aether/utils/thread_utils.hpp`.

`empty()`, `full()`, and `size_approx()` are observations. Under concurrent producer/consumer activity, their results are not transactional promises that a later push or pop must succeed.

## Cache-line padding and false sharing

`head_` and `tail_` are stored in separate `aether::detail::CachePadded<std::atomic<std::size_t>>` objects. `CachePadded<T>` is defined in `include/aether/detail/cache_line.hpp` and is aligned to `aether::detail::cache_line_size`, currently `64` bytes.

The reason is false-sharing avoidance. The producer frequently writes `head_`, while the consumer frequently writes `tail_`. If both atomics lived on the same cache line, two CPU cores could repeatedly invalidate each other's cache line even though they mostly own different counters. Separating the counters makes that layout intention explicit.

This is a design choice, not a standalone performance claim. Phase 5 now provides benchmark infrastructure for measuring SPSC behavior, but any reported performance interpretation must be tied to raw benchmark output and the documented methodology.

## Correctness tests

Current tests and tools relevant to the SPSC queue:

- `tests/test_spsc_basic.cpp`: checks empty/full behavior, exact capacity usage, FIFO order, `size_approx()`, empty pop failure, full push failure, small-capacity behavior, and `MessageHeader` transfer.
- `tests/test_spsc_wraparound.cpp`: checks repeated fill/drain cycles, interleaved wraparound behavior, and rolling push/pop behavior through many logical counter advances.
- `tests/test_spsc_concurrent.cpp`: transfers ordered values between one producer thread and one consumer thread, and checks backpressure behavior after filling the queue.
- `tests/test_spsc_move_only.cpp`: verifies move-only payload support through `std::unique_ptr<int>` and a custom move-only, non-default-constructible type.
- `tests/test_spsc_stress.cpp`: runs ordered-transfer stress coverage across multiple compile-time capacities.
- `tools/stress_spsc.cpp`: provides a manual stress-validation tool with selectable message counts and supported capacities. It reports elapsed time and retry counts for operator visibility, but it is not a benchmark framework and its output is not a benchmark result.

Phase 5 benchmark files relevant to this queue are:

- `benchmarks/bench_spsc_throughput.cpp`: measures ordered producer/consumer throughput across queue capacities.
- `benchmarks/bench_spsc_latency.cpp`: records approximate timestamped transfer latency distributions.
- `benchmarks/bench_payload_sizes.cpp`: compares fixed-size payload objects across selected capacities.
- `scripts/run_benchmarks.sh`: runs the Release benchmark workflow and stores raw outputs under `benchmark-results/`.

These benchmarks complement correctness tests; they do not replace them.

## Limitations

Current limitations are intentional for this phase:

- SPSC only: exactly one producer and one consumer.
- No blocking wait API.
- No batch push/pop API.
- No zero-copy reservation/commit API.
- No persistence or cross-process storage.
- No official measured performance results are committed in `docs/performance-results.md` yet.
- `size_approx()` is approximate under concurrency and is not a synchronization barrier.

Multiple producers or multiple consumers would break the ownership assumptions around `head_` and `tail_`. Use this queue only under the one-producer/one-consumer contract.

## Future work

Phase 5 has added the benchmark framework and honest reporting workflow. Future ring-buffer-related work should use those benchmarks to compare changes against the existing baseline without inventing latency or throughput claims.

Phase 6 has added the standalone mmap file primitive. The SPSC queue remains independent from persistence and does not directly own mapped storage. Phase 8 has added the first broker layer that composes this queue with optional WAL persistence. Future queue-specific work should still keep the SPSC primitive independent and benchmark changes against the existing baseline.

Optional advanced queue features, such as batching or zero-copy reservation APIs, should be considered only after measurement shows a real need and after their correctness model is designed carefully.
