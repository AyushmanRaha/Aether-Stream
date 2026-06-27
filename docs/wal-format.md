# Aether-Stream WAL Format

## Purpose and scope

The Phase 7 write-ahead log (WAL) is an append-only local persistence format for Aether-Stream message records. Phase 8 now uses this WAL through `aether::PersistentBroker<T, Capacity>`, which serializes trivially copyable event objects into WAL payloads before publishing them to the in-memory SPSC queue.

This document remains the binary WAL format specification. Broker-level semantics, typed replay, and user-facing API behavior are documented in `docs/broker-api.md`.

The WAL format itself is still generic `MessageView` payload storage. It does not know about C++ event types; `PersistentBroker` is the typed layer that maps trivially copyable objects to payload bytes.

## File model

A WAL file is created at a fixed size and mapped with `aether::io::MmapFile`. Writers append records sequentially from offset zero. Unused bytes after the last complete record remain zero-filled in a newly created file.

## Record layout

Each record is a fixed 40-byte header followed immediately by `payload_size` payload bytes. All integer fields are serialized in little-endian byte order. The on-disk header is serialized explicitly; readers and writers do not rely on raw struct `memcpy` for the stable format.

| Offset | Field | Type | Size | Notes |
| ---: | --- | --- | ---: | --- |
| 0 | `magic` | `uint32_t` | 4 | ASCII bytes `AWAL` |
| 4 | `version` | `uint16_t` | 2 | Format version `1` |
| 6 | `header_size` | `uint16_t` | 2 | Always `40` for v1 |
| 8 | `payload_size` | `uint32_t` | 4 | Number of payload bytes |
| 12 | reserved/padding | bytes | 4 | Must serialize as zero |
| 16 | `sequence` | `uint64_t` | 8 | Writer-assigned sequence |
| 24 | `timestamp_ns` | `uint64_t` | 8 | Message timestamp or monotonic append timestamp |
| 32 | `checksum` | `uint32_t` | 4 | CRC32 checksum |
| 36 | `flags` | `uint32_t` | 4 | Message flags |

`record_total_size = 40 + payload_size`.

## Constants

- Magic bytes: `AWAL`.
- Format version: `1`.
- Header size: `40` bytes.
- Reserved bytes at offsets 12 through 15 must be zero in serialized output.

## Checksum policy

Records use standard IEEE CRC32:

- polynomial: `0xEDB88320`;
- initial value: `0xFFFFFFFF`;
- final XOR: `0xFFFFFFFF`.

The checksum is computed over the serialized 40-byte header with the `checksum` field set to zero, followed by the payload bytes. The reader validates the checksum before returning a record. A checksum mismatch returns `StatusCode::corrupted_record`.

## Reader behavior

The Phase 7 reader performs a sequential scan from the current offset:

- a zero-filled tail after the last valid record is clean EOF and returns `StatusCode::empty`;
- an incomplete header or incomplete payload at the tail is treated as a clean stop and returns `StatusCode::empty` with partial-record detail;
- invalid magic, unsupported version, or invalid header size returns `StatusCode::corrupted_record`;
- checksum mismatch returns `StatusCode::corrupted_record`;
- the reader never reads beyond the mapped file bounds.

## Phase 8 broker integration

`aether::PersistentBroker<T, Capacity>` uses the WAL writer with WAL-before-queue semantics:

1. Check broker validity.
2. Check queue capacity.
3. Append the serialized object representation of `T` to the WAL.
4. Publish the value to the in-memory SPSC queue only after WAL append succeeds.

For typed replay, `PersistentBroker<T, Capacity>::replay(path, visitor)` opens the WAL reader, validates records using the normal WAL reader path, checks that each payload size equals `sizeof(T)`, reconstructs a local `T` with `std::memcpy`, and calls the visitor.

This typed replay is intended for same-program/same-platform replay of trivially copyable event structs. Cross-language schemas, ABI-independent persistence, endian conversion for typed payloads, and schema evolution are not part of Phase 8.

## Limitations

Phase 8 broker integration exists through `PersistentBroker`, but the lower-level WAL reader/writer remain standalone and reusable.

The WAL layer still intentionally does not include:

- file rotation;
- multi-segment WAL files;
- concurrent writer support;
- recovery indexes;
- CLI tools;
- schema evolution for typed broker payloads;
- cross-language or ABI-independent typed payload encoding;
- production crash-recovery guarantees beyond the existing explicit flush behavior.
