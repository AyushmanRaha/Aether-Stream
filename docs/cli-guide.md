# Aether-Stream CLI Guide

## CLI status

Dependency-free CLI apps make the existing in-process broker and WAL components runnable from a terminal. CLI output includes concise metrics summaries. These tools are demos and debugging aids, not production services.

## Build

```sh
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DAETHER_BUILD_TESTS=ON \
  -DAETHER_BUILD_EXAMPLES=ON \
  -DAETHER_BUILD_TOOLS=ON \
  -DAETHER_BUILD_APPS=ON
cmake --build build/release
ctest --test-dir build/release --output-on-failure
```

## Demo flow

```sh
./build/release/apps/aether_bench --messages 100000 --payload-size 64 --capacity 1024
./build/release/apps/aether_pub --wal data/sample.wal --messages 1000
./build/release/apps/aether_inspect_wal --wal data/sample.wal
./build/release/apps/aether_replay --wal data/sample.wal --limit 10
./build/release/apps/aether_sub --wal data/sample.wal --limit 10
```

## `aether_bench`

Purpose: run a convenient two-thread local `Broker` demo benchmark without Google Benchmark.

Options: `--messages <N>`, `--payload-size <8|32|64|256|1024>`, `--capacity <64|256|1024|4096|65536>`, `--output-dir <path>`, `--help`.

Example:

```sh
./build/release/apps/aether_bench --messages 100000 --payload-size 64 --capacity 1024
```

Expected output shape: messages, payload size, capacity, elapsed seconds, messages/sec, producer full retries, consumer empty retries, `metrics.published`, `metrics.consumed`, `metrics.publish_failed_full`, `metrics.consume_failed_empty`, and a result-file path.

Limitations: this is a CLI demo benchmark, not a replacement for the Google Benchmark suite or official performance results. Do not publish numbers without raw measured outputs and context.

## `aether_pub`

Purpose: write generated trivially copyable `OrderEvent` records to a local WAL-backed `PersistentBroker`.

Options: `--wal <path>`, `--messages <N>`, `--wal-size <bytes>`, `--flush-on-commit`, `--help`.

Example:

```sh
./build/release/apps/aether_pub --wal data/sample.wal --messages 1000
```

Expected output shape: WAL path, messages requested, records written, `metrics.published`, `metrics.wal_records_written`, `metrics.wal_bytes_written`, `metrics.wal_flushes`, `metrics.publish_failed_full`, `metrics.wal_failures`, next sequence, current WAL offset, and remaining WAL space.

Limitations: generated demo payloads only; no networking, no inter-process broker service, and no production persistence claims.

## `aether_sub`

Purpose: demonstrate local consumption. Without `--wal`, it publishes generated `OrderEvent` values to an in-process `Broker` and consumes them. With `--wal`, it replays typed `OrderEvent` records written by `aether_pub`.

Options: `--messages <N>`, `--capacity <64|256|1024|4096|65536>`, `--limit <N>`, `--wal <path>`, `--help`.

Example:

```sh
./build/release/apps/aether_sub --wal data/sample.wal --limit 10
```

Expected output shape: an explicit no-networking notice and either:

- local mode: events up to `--limit`, a produced/consumed/suppressed summary, `metrics.published`, `metrics.consumed`, `metrics.publish_failed_full`, and `metrics.consume_failed_empty`; or
- WAL replay mode: events up to `--limit`, a replay summary, `metrics.recovered_records`, and `metrics.recovery_failures`.

Limitations: no live inter-process broker connection and no remote subscriber mode.

## `aether_replay`

Purpose: generic raw WAL replay using `WalReader` with printable payload previews.

Options: `--wal <path>`, `--limit <N>`, `--payload-preview-bytes <N>`, `--help`.

Example:

```sh
./build/release/apps/aether_replay --wal data/sample.wal --limit 10
```

Expected output shape: one line per record with offset, sequence, timestamp, payload size, flags, checksum, and preview, followed by records replayed, final offset, `metrics.recovered_records`, and `metrics.recovery_failures`.

Limitations: payload previews are raw bytes; typed interpretation belongs to `aether_sub --wal` for demo `OrderEvent` files.

## `aether_inspect_wal`

Purpose: scan and summarize WAL files for demo/debugging, including corrupt WAL investigation.

Options: `--wal <path>`, `--limit <N>`, `--verbose`, `--help`.

Example:

```sh
./build/release/apps/aether_inspect_wal --wal data/sample.wal --verbose --limit 5
```

Expected output shape: WAL format constants, optional per-record lines, record count, total payload bytes, first sequence, last sequence, final reader offset, clean/corrupt scan status, `metrics.recovered_records`, and `metrics.recovery_failures`.

Limitations: scanner only reports the existing WAL format; it does not repair files.

## Explicit limitations

- No networking.
- No live inter-process broker connection.
- `aether_sub --wal` replays demo `OrderEvent` records written by `aether_pub`.
- `aether_replay` is generic raw WAL replay.
- `aether_bench` is a convenient CLI benchmark/demo, not a replacement for the Google Benchmark suite or official performance results.
- No official benchmark results or fake performance numbers are committed.
- No production persistence claims.
