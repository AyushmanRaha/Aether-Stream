# Memory Ordering

## Current status

This document explains the acquire/release protocol used by `aether::SpscRingBuffer<T, Capacity>` in `include/aether/spsc_ring_buffer.hpp` as of Phase 4.

The queue is strictly single-producer/single-consumer. The memory-ordering strategy depends on that ownership model.

## The problem

The queue stores payloads in non-atomic raw slots, but uses atomic counters to publish which slots are ready or reusable:

- `head_`: producer-published, consumer-observed;
- `tail_`: consumer-published, producer-observed.

The consumer must not read a slot before the producer's construction of that slot is visible. The producer must not reuse a slot before the consumer's move-from and destruction of that slot are visible.

The slot data itself is not atomic. Correctness comes from publishing slot lifecycle transitions through atomic `head_` and `tail_` updates.

## Ownership model

The SPSC ownership rule is simple:

- the producer owns advancing `head_`;
- the consumer owns advancing `tail_`;
- the consumer may observe `head_`, but must not write it;
- the producer may observe `tail_`, but must not write it.

This is why the SPSC queue is simpler than MPSC or MPMC designs. There is no race between multiple producers trying to claim the same `head_` value, and no race between multiple consumers trying to claim the same `tail_` value.

## Atomic operations table

| Location | Operation | Memory order | Why |
| --- | --- | --- | --- |
| Producer in `try_emplace()` | Load `head_` | `std::memory_order_relaxed` | The producer is the only thread that advances `head_`, so it can read its owned counter without synchronization. |
| Producer in `try_emplace()` | Load `tail_` | `std::memory_order_acquire` | Observes the consumer's release-store to `tail_` after slot destruction before deciding a slot is reusable. |
| Producer in `try_emplace()` | Store `head + 1` to `head_` | `std::memory_order_release` | Publishes the newly constructed slot to the consumer. |
| Consumer in `try_pop()` | Load `tail_` | `std::memory_order_relaxed` | The consumer is the only thread that advances `tail_`, so it can read its owned counter without synchronization. |
| Consumer in `try_pop()` | Load `head_` | `std::memory_order_acquire` | Observes the producer's release-store to `head_` after slot construction before reading the slot. |
| Consumer in `try_pop()` | Store `tail + 1` to `tail_` | `std::memory_order_release` | Publishes that the consumed slot's lifetime has ended and that storage can be reused. |
| `empty()` | Load `head_` and `tail_` | `std::memory_order_acquire` | Provides an observational status check; it is not a cross-thread transaction. |
| `full()` | Uses `size_approx()` | Acquire loads through `size_approx()` | Provides an observational status check based on approximate size. |
| `size_approx()` | Load `head_` and `tail_` | `std::memory_order_acquire` | Provides an approximate concurrent observation, not a synchronization barrier for payload access. |

## Producer publication flow

The producer path is implemented by `try_emplace()`, and `try_push()` forwards into it.

1. Read local `head_` with `std::memory_order_relaxed`.
2. Acquire-read consumer-published `tail_` with `std::memory_order_acquire`.
3. Check space with `head - tail >= Capacity`.
4. If there is space, construct the object in `buffer_[head & mask_]` with `std::construct_at(...)`.
5. Release-store `head + 1` to `head_` with `std::memory_order_release`.

The release store is the publication point: it says, “the object in this slot has been constructed and may now be consumed.”

## Consumer publication flow

The consumer path is implemented by `try_pop(T& out)`.

1. Read local `tail_` with `std::memory_order_relaxed`.
2. Acquire-read producer-published `head_` with `std::memory_order_acquire`.
3. Check availability with `tail == head`.
4. If an item is available, move from `buffer_[tail & mask_]` into `out`.
5. Destroy the object in the slot with `std::destroy_at(...)`.
6. Release-store `tail + 1` to `tail_` with `std::memory_order_release`.

The release store is the reuse point: it says, “the object lifetime in this slot has ended and the producer may reuse the storage after observing this tail value.”

## Happens-before explanation

There are two important synchronization pairs:

1. Producer `head_` release-store synchronizes with consumer `head_` acquire-load.
   - The producer's slot construction happens-before the consumer's slot read.
2. Consumer `tail_` release-store synchronizes with producer `tail_` acquire-load.
   - The consumer's slot destruction happens-before the producer's later reuse of that slot.

These edges protect non-atomic slot data by ordering object construction, payload movement, destruction, and reuse.

```text
producer: construct slot  -> release-store head
                              synchronizes-with
consumer: acquire-load head -> move from slot -> destroy slot -> release-store tail
                                                                  synchronizes-with
producer: acquire-load tail -> reuse slot storage
```

## Why not memory_order_seq_cst everywhere?

`std::memory_order_seq_cst` is often simpler to explain because it gives all sequentially consistent atomic operations one global order. That is stronger than this queue needs.

The queue needs two publication handoffs:

- producer publishes constructed payloads through `head_`;
- consumer publishes reusable storage through `tail_`.

Acquire/release exactly models those handoffs. The project uses acquire/release here because it expresses the required happens-before relationships directly, without requiring a stronger global sequentially consistent order. This is a correctness explanation, not a benchmark claim.

## Why not relaxed everywhere?

Relaxed atomics would prevent data races on the counters themselves, but would not establish the payload visibility rules the queue needs.

If the producer release-store to `head_` and consumer acquire-load of `head_` were removed, the consumer could observe the counter update without the necessary guarantee that slot construction is visible. If the consumer release-store to `tail_` and producer acquire-load of `tail_` were removed, the producer could reuse storage without the necessary guarantee that the previous object destruction is visible.

The raw payload slots are not atomic. They must be protected by the acquire/release publication protocol.

## What would break this design?

The design would be invalidated by:

- multiple producer threads calling `try_push()` or `try_emplace()`;
- multiple consumer threads calling `try_pop()`;
- reading or writing queue slots outside the queue protocol;
- treating `size_approx()` as a synchronization barrier or as a reservation mechanism;
- changing memory orders without re-running concurrent and stress validation.

Multiple producers would race to own and advance `head_`. Multiple consumers would race to own and advance `tail_`. Those are different algorithms and require different coordination.

## Testing and verification

Relevant verification files:

- `tests/test_spsc_basic.cpp` checks basic FIFO, full/empty, exact capacity, and `MessageHeader` behavior.
- `tests/test_spsc_wraparound.cpp` checks logical counter wraparound across physical slots.
- `tests/test_spsc_concurrent.cpp` checks ordered transfer between one producer thread and one consumer thread.
- `tests/test_spsc_move_only.cpp` checks move-only and non-default-constructible payload behavior.
- `tests/test_spsc_stress.cpp` checks repeated ordered transfer across multiple capacities.
- `tools/stress_spsc.cpp` is a manual stress-validation tool, not a benchmark result source.

Repeated runs are useful because concurrency bugs can be intermittent. Sanitizer jobs may be added in later phases, but this documentation task does not add sanitizer workflow files or CI.

## Plain-English summary

For interviews and code review, the short version is:

- the producer constructs a slot, then release-publishes the new `head_`;
- the consumer acquire-observes `head_`, then reads and moves the slot payload;
- the consumer destroys the slot object, then release-publishes the new `tail_`;
- the producer acquire-observes `tail_`, then reuses the slot storage.

Relaxed loads are used only for the thread-owned counter. Acquire/release is used whenever one thread observes the other thread's publication counter.
