# Metrics, diagnostics, and observability

Aether-Stream includes lightweight always-on broker counters, immutable metrics snapshots, a diagnostic latency histogram, CLI metrics summaries, and an end-to-end broker benchmark. These features make broker behavior visible without changing the SPSC queue algorithm or WAL record format. In plain terms, metrics are a small built-in dashboard: counters track what the broker has done, and the latency histogram tracks how long operations took, without changing how the queue or WAL behave.

## Snapshot API

Both `Broker<T, Capacity>` and `PersistentBroker<T, Capacity>` expose:

```cpp
auto metrics = broker.metrics_snapshot(); // preferred explicit name
auto same = broker.snapshot();            // convenience alias
broker.reset_metrics();                   // reset this broker's counters
```

Snapshots are value copies. Updating broker counters after a snapshot is taken does not mutate the old snapshot.

## BrokerMetricsSnapshot fields

`aether::metrics::BrokerMetricsSnapshot` contains:

- `published`: successful publish operations that made a message visible to the consumer.
- `consumed`: successful consume operations.
- `publish_failed_full`: publish attempts rejected because the SPSC queue was full.
- `consume_failed_empty`: consume attempts rejected because the SPSC queue was empty.
- `publish_failed_invalid`: publish attempts rejected because the broker/config/open state was invalid.
- `consume_failed_invalid`: consume attempts rejected because the broker/config/open state was invalid.
- `wal_bytes_written`: bytes appended by the persistent broker publish path.
- `wal_records_written`: WAL records appended by the persistent broker publish path.
- `wal_flushes`: successful explicit `flush()` calls.
- `wal_failures`: WAL append, flush, or post-WAL queue-publication failures detected by `PersistentBroker`.
- `recovered_records`: typed replay or WAL inspection records successfully processed.
- `recovery_failures`: replay/open/corruption/visitor failures.

## Counter design

`aether::metrics::BrokerCounters` is header-only and uses `std::atomic<std::uint64_t>` with relaxed memory ordering. Relaxed atomics keep increments cheap and avoid locks in publish/consume hot paths. These counters are observability signals, not a correctness mechanism; broker correctness still comes from the SPSC queue and WAL protocols.

Metrics remain approximate under concurrent observation. Read snapshots after producer/consumer threads join when an exact run summary is required.

The SPSC limitation remains unchanged: `Broker` and `PersistentBroker` are still designed for exactly one producer and exactly one consumer.

## WAL metrics

`PersistentBroker` preserves WAL-before-queue semantics: a persistent publish appends to the WAL before publishing to the in-memory queue. `wal_bytes_written` is computed from WAL writer offset deltas for successful persistent publish appends. `wal_records_written` counts successful WAL appends through the persistent broker publish path. `wal_flushes` counts successful explicit `flush()` calls; failed appends and flushes increment `wal_failures`.

## Recovery metrics

Typed replay can collect recovery metrics with:

```cpp
aether::metrics::BrokerCounters counters;
auto status = aether::PersistentBroker<Event, 1024>::replay_with_metrics(
    path,
    [](const Event& event, const aether::wal::WalRecordHeader& header) {
        (void)event;
        (void)header;
        return aether::Status::ok();
    },
    counters);
auto metrics = counters.snapshot();
```

The existing `replay(path, visitor)` API is preserved for callers that do not need explicit replay metrics.

## LatencyHistogram

`aether::metrics::LatencyHistogram` stores latency samples in nanoseconds and is intended for diagnostics, tests, and benchmark helpers. It is not thread-safe; callers must externally synchronize if sharing one histogram across threads. It is not wired into broker hot paths because storing samples can allocate.

Percentiles use deterministic nearest-rank behavior:

1. Empty histograms return `0` for percentiles and `0.0` for `mean()`.
2. Percentiles `<= 0` clamp to `min()`.
3. Percentiles `>= 100` clamp to `max()`.
4. Otherwise, a copy of the samples is sorted and `ceil((p / 100) * N) - 1` selects the sample.

Example:

```cpp
aether::metrics::LatencyHistogram hist;
hist.reserve(1000);
hist.record(125);
hist.record(250);
auto p50 = hist.p50();
auto p99 = hist.p99();
```

## In-memory broker example

```cpp
aether::Broker<int, 1024> broker;
broker.try_publish(42);
int out{};
broker.try_consume(out);

auto metrics = broker.metrics_snapshot();
// metrics.published == 1, metrics.consumed == 1
```

## Persistent broker example

```cpp
aether::WalConfig wal{};
wal.path = "data/sample.wal";
wal.file_size_bytes = 1024 * 1024;

aether::PersistentBroker<MyEvent, 1024> broker(wal);
broker.try_publish(MyEvent{});
broker.flush();

auto metrics = broker.snapshot();
// Inspect metrics.published, metrics.wal_records_written, metrics.wal_bytes_written,
// metrics.wal_flushes, and metrics.wal_failures.
```

## CLI and benchmark output

CLI apps print concise metrics summaries. The `bench_broker_end_to_end` benchmark target exercises publish-to-consume paths with WAL disabled and enabled. These diagnostics do not create official performance claims; committed benchmark result documents remain templates unless raw controlled measurements are intentionally added later.
