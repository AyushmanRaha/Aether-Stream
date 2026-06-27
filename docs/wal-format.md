# Aether-Stream WAL Format

## Purpose and scope

The Phase 7 write-ahead log (WAL) is an append-only local persistence format for Aether-Stream message records. It is implemented as a preallocated mmap-backed file and is intended as a foundation for later broker integration.

This document describes Phase 7 only. The WAL is not yet integrated with a broker runtime, recovery index, CLI, file rotation, or multi-segment persistence system.

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

## Limitations

Phase 7 intentionally does not include:

- broker integration;
- file rotation;
- multi-segment WAL files;
- concurrent writer support;
- recovery indexes;
- CLI tools.
