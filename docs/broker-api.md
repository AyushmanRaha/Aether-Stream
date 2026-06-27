# Broker API

Phase 8 exposes the developer-facing broker API for Aether-Stream. The product-facing API is now a typed in-memory broker over the existing SPSC queue, plus a WAL-backed persistent broker that appends records before publishing them to the in-memory queue.

## Status

Broker operations return `aether::Status` instead of throwing exceptions for normal full, empty, configuration, WAL, or corruption outcomes. Check `status.is_ok()` or the explicit `bool` conversion before assuming an operation succeeded.

Common Phase 8 statuses include:

- `StatusCode::ok`: operation succeeded.
- `StatusCode::full`: `try_publish` could not enqueue because the queue is full.
- `StatusCode::empty`: `try_consume` found no queued value.
- `StatusCode::invalid_argument`: invalid runtime configuration or unopened persistent broker.
- `StatusCode::out_of_space`: WAL append could not fit in the configured file.
- `StatusCode::corrupted_record`: replay found a WAL record that does not match the expected typed payload.

## In-memory broker

`aether::Broker<T, Capacity>` wraps `SpscRingBuffer<T, Capacity>` behind the broker-oriented `try_publish`, `try_emplace`, and `try_consume` API.

```cpp
#include <aether/broker.hpp>
#include <cstdint>

struct OrderEvent {
    std::uint64_t id{};
    double price{};
    std::uint32_t quantity{};
};

aether::Broker<OrderEvent, 65536> broker;

const auto publish_status = broker.try_publish(OrderEvent{1, 101.25, 10});
OrderEvent out{};
const auto consume_status = broker.try_consume(out);
```

- `try_publish(value)` copies or moves one value into the SPSC queue.
- `try_emplace(args...)` constructs one queued value in place.
- `try_consume(out)` moves the next queued value into `out`.
- A full queue returns `StatusCode::full`; it does not block.
- An empty queue returns `StatusCode::empty`; it does not block.

## Persistent broker

`aether::PersistentBroker<T, Capacity>` combines the same in-memory broker API with the Phase 7 append-only WAL writer.

```cpp
#include <aether/persistent_broker.hpp>
#include <cstdint>

struct OrderEvent {
    std::uint64_t id{};
    double price{};
    std::uint32_t quantity{};
};

aether::WalConfig wal{};
wal.path = "orders.wal";
wal.file_size_bytes = 64ull * 1024ull * 1024ull;
wal.flush_on_commit = false;

aether::PersistentBroker<OrderEvent, 65536> broker(wal);
const auto status = broker.try_publish(OrderEvent{1, 101.25, 10});
```

`T` must be trivially copyable. Phase 8 typed persistence serializes `T` by copying its C++ object representation into the WAL payload and reconstructs it with `std::memcpy` during typed replay. This is intended for same-program/same-platform replay of simple event structs. Stable cross-language schemas, schema evolution, endian conversion, and ABI-independent persistence are not part of Phase 8.

## Durability semantics

`PersistentBroker` uses WAL-before-queue semantics:

1. `try_publish(value)` first checks that the broker is open and valid.
2. It checks queue capacity before writing to the WAL.
3. If the queue is full, it returns `StatusCode::full` and does not append a WAL record.
4. If the queue is not full, it appends the serialized value to the WAL first.
5. If WAL append fails, it returns the WAL error and does not publish to the queue.
6. If WAL append succeeds, it publishes the value to the in-memory SPSC queue.

A successful persistent `try_publish` means the value was appended to the WAL before it became visible to the consumer. Phase 8 uses the existing WAL flush behavior: `flush_on_commit=true` requests a flush on each append, and `flush()` can be called explicitly. Do not treat this as a blanket crash-proof fsync guarantee without considering the configured flush policy and platform storage semantics.

## Concurrency model

Both broker types are SPSC only: exactly one producer may call publish operations, and exactly one consumer may call consume operations. Multiple producers or multiple consumers are unsupported. Phase 8 intentionally does not add blocking publish/consume APIs; callers should handle `StatusCode::full` and `StatusCode::empty` explicitly.

## Configuration

The queue capacity is a compile-time template argument and must be a power of two with a minimum of 2. If runtime `QueueConfig` or `BrokerConfig` is supplied, its `queue.capacity` must match the broker template capacity exactly.

For persistent brokers:

- `BrokerConfig::enable_wal` must be `true`.
- `WalConfig::path` must not be empty.
- `WalConfig::file_size_bytes` must be large enough for at least one broker record: WAL header size plus `sizeof(T)`.

## Replay/recovery

Typed replay is provided by:

```cpp
aether::PersistentBroker<OrderEvent, 65536>::replay(
    "orders.wal", [](const OrderEvent& event, const aether::wal::WalRecordHeader& header) {
        // inspect event and header.sequence/header.flags
        return aether::Status::ok();
    });
```

The replay helper opens the WAL, validates each record through the WAL reader, checks that the payload size equals `sizeof(T)`, reconstructs a local `T`, and calls the visitor. A visitor status failure stops replay and is returned to the caller.

For terminal demos, see `docs/cli-guide.md`. The CLI tools exercise the local broker/WAL APIs but do not change the broker's SPSC-only or no-networking semantics.

## Limitations

- No networking.
- Phase 9 CLI demos exist, but there is still no networked broker service or live inter-process subscriber.
- No metrics yet.
- No MPSC or MPMC support.
- No blocking broker API.
- No schema evolution for persisted C++ structs yet.
- No production-ready crash-recovery guarantees.
