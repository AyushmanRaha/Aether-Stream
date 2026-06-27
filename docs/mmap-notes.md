# Memory-mapped file notes

Phase 6 adds a small memory-mapped file primitive for Aether-Stream. It is intentionally only an operating-system resource wrapper; it is not a WAL, a broker, or a durability policy.

Historical note: this document describes the Phase 6 mmap layer. Phase 7 has since added standalone WAL record, writer, reader, checksum, and replay support on top of `MmapFile`. The separation is intentional: `MmapFile` remains a low-level mapped-file primitive, while WAL record semantics live under `include/aether/wal/` and `src/wal/`.

## What `mmap` means

`mmap` asks the operating system to map a file into a process address range. After a successful mapping, code can read and write the mapped bytes through a pointer or span instead of calling `read` and `write` for each access. The kernel keeps the mapping connected to the underlying file.

## Why this layer comes before WAL

Aether-Stream is planned to include mmap-backed persistence in later phases. Before adding WAL records, replay, recovery, or broker semantics, raw POSIX file descriptors and mapping calls need to be isolated behind a small, testable RAII class. `aether::io::MmapFile` provides that boundary so later WAL code can work with mapped bytes instead of direct OS calls.

## `MmapFile` lifecycle

`MmapFile` follows an explicit RAII lifecycle:

1. `create(path, size)` opens or creates a file, truncates it to a nonzero size, and maps it read/write.
2. `open_existing(path)` opens an existing nonzero-size file and maps it read/write.
3. Callers access mapped storage through `data()` or `bytes()`.
4. `flush()` calls `msync(..., MS_SYNC)` to ask the OS to synchronously flush dirty mapped pages to the file.
5. `resize(new_size)` flushes, unmaps, resizes the file with `ftruncate`, and remaps it.
6. `close()` flushes when configured, unmaps, closes the file descriptor, and leaves the object invalid.
7. The destructor is `noexcept`, calls `close()`, and ignores any returned error because destructors cannot reliably report failures.

## Nonzero file sizes

POSIX mappings need an address range backed by file storage. A zero-byte file has no bytes to map for this use case, so `MmapFile` rejects zero-sized creates and zero-sized existing files. Create files at the required initial size before mapping them.

## `MAP_SHARED`

`MmapFile` uses `MAP_SHARED`, which means writes to the mapped region are shared with the underlying file mapping rather than being private copy-on-write changes. This is the mode later persistence code needs as a primitive, but it does not by itself define WAL record layout or crash-recovery behavior.

## `msync(..., MS_SYNC)`

`flush()` uses `msync` with `MS_SYNC`. This asks the OS to synchronously flush modified mapped pages for the mapping before returning. It is useful when code wants an explicit success or error status for flushing mapped bytes.

Destructor flushing is best-effort. The destructor cannot throw or return a `Status`, so callers that need to observe flush failures should call `flush()` or `close()` explicitly and check the returned `Status`.

## Page size basics

Operating systems manage mappings using memory pages. Callers do not need to request a file size that exactly equals the system page size; a 4096-byte test file and an 8192-byte resized file are both ordinary requests. Internally, the OS backs mappings with page-sized units and handles the details.

## Platform scope

Phase 6 focuses on POSIX mmap behavior for macOS and Linux. The public class keeps POSIX headers out of the public header. Non-POSIX platforms receive a clear `io_error` fallback instead of a Windows mapping implementation.

## Phase 6 non-goals

Phase 6 deliberately does not add:

- a WAL binary format;
- WAL writer, reader, replay, or recovery logic;
- checksums;
- crash recovery claims;
- broker durability semantics;
- broker APIs;
- CLI tools;
- metrics or diagnostics;
- performance claims.

Phase 7 now builds the standalone WAL writer and reader on top of `MmapFile` without mixing WAL record semantics into this low-level mapping primitive.
