# Ring Buffer Design

## Current status

This document describes the `aether::SpscRingBuffer<T, Capacity>` implementation in `include/aether/spsc_ring_buffer.hpp`. `BatchBroker` and experimental `ZeroCopySpsc` build around related local messaging needs, but the core `SpscRingBuffer` design remains intentionally SPSC, independent, and reusable.

## Scope and non-goals

Supported:

- exactly one producer thread calling `try_push()` or `try_emplace()`;
- exactly one consumer thread calling `try_pop()`;
- compile-time fixed-capacity power-of-two storage;
- non-blocking push/pop attempts returning `bool`;
- copyable, movable, move-only, and non-default-constructible payloads when operation constraints are satisfied.

Unsupported:

- MPSC or MPMC queues;
- blocking wait APIs;
- cross-process queues;
- persistence, WAL storage, network transport, or direct mmap integration inside the queue;
- official performance claims unless raw benchmark output has been published.

The manual stress tool is for correctness validation only. Performance results must come from preserved `./scripts/run_benchmarks.sh` evidence.

## Public API overview

```cpp
#include <aether/spsc_ring_buffer.hpp>

#include <iostream>

int main() {
    aether::SpscRingBuffer<int, 1024> queue;
    queue.try_push(42);
    queue.try_emplace(43);

    int value = 0;
    while (queue.try_pop(value)) {
        std::cout << value << '\n';
    }
}
```

Core operations are `try_push`, `try_emplace`, `try_pop`, `empty`, `full`, `capacity`, and `size_approx`.

## Core design

The queue stores elements in raw slots and tracks progress with monotonic logical counters:

- producer position: `head_`;
- consumer position: `tail_`;
- physical slot: `logical_index & (Capacity - 1)`.

Because counters are logical rather than just wrapped indexes:

- empty means `head == tail`;
- full means `head - tail >= Capacity`;
- all `Capacity` slots are usable.

## Head/tail and slot lifecycle

```mermaid
flowchart LR
    Producer["Producer thread<br/>owns head_"] --> CheckSpace["Acquire-load tail_<br/>check free space"]
    CheckSpace --> Construct["Construct payload in<br/>buffer[head & mask]"]
    Construct --> PublishHead["Release-store head_ + 1"]
    PublishHead --> ConsumerAcquire["Consumer acquire-loads head_"]
    ConsumerAcquire --> Consume["Move payload out<br/>destroy slot"]
    Consume --> PublishTail["Release-store tail_ + 1"]
    PublishTail --> ProducerReuse["Producer acquire-loads tail_<br/>slot reusable"]
```

Slot states follow this cycle:

1. Empty raw storage.
2. Producer constructs `T` in-place.
3. Producer release-publishes `head_`.
4. Consumer acquire-observes `head_` and moves the object out.
5. Consumer destroys the object.
6. Consumer release-publishes `tail_`.
7. Producer acquire-observes `tail_` and may reuse the slot.

## Memory ordering summary

The owner of a counter can use relaxed operations for local observations. Cross-thread publication uses acquire/release:

| Operation | Memory order | Why |
|---|---|---|
| Producer local `head_` load | relaxed | Only producer advances `head_`. |
| Producer observes `tail_` | acquire | Must see consumer's slot destruction before reuse. |
| Producer publishes `head_` | release | Makes constructed payload visible to consumer. |
| Consumer observes `head_` | acquire | Must see producer's constructed payload. |
| Consumer local `tail_` load | relaxed | Only consumer advances `tail_`. |
| Consumer publishes `tail_` | release | Makes slot destruction visible to producer. |

## Relationship to related APIs

`BatchBroker` batches broker-level calls over queue operations. `ZeroCopySpsc` explores reservation/commit semantics. Neither changes the original `SpscRingBuffer` contract: this queue remains SPSC only and does not become a cross-process, blocking, MPSC, or MPMC primitive.

## Testing and verification

Relevant files:

- `tests/test_spsc_basic.cpp`
- `tests/test_spsc_wraparound.cpp`
- `tests/test_spsc_concurrent.cpp`
- `tests/test_spsc_move_only.cpp`
- `tests/test_spsc_stress.cpp`
- `tools/stress_spsc.cpp`

CI, sanitizer workflows, clang-tidy integration, and package verification improve confidence, but they do not justify production-readiness claims.

## Plain-English summary

A bounded SPSC ring buffer works well here because the one-producer/one-consumer contract lets each thread own one logical counter. The producer constructs a payload, then release-stores `head_`; the consumer acquire-loads `head_`, moves and destroys the payload, then release-stores `tail_`. The producer acquire-loads `tail_` before reusing storage. Full and empty are distinguished with monotonic counters, so all slots can be used.
