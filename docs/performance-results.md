# Performance Results

Aether-Stream publishes benchmark results only when the evidence is preserved with environment details, command provenance, and caveats. For methodology and publication rules, see [Benchmark methodology](benchmark-methodology.md). Return to the [README](../README.md) for the project overview.

## Local M1 MacBook Air run — 2026-06-29

Detailed consolidated results are in [M1 MacBook Air benchmark run — 2026-06-29](benchmark-results/m1-macbook-air-2026-06-29.md). The detailed page preserves the benchmark rows and counters in one redacted Markdown transcript rather than committing separate raw `.txt` or `.json` files.

### Environment summary

| Field | Value |
|---|---|
| Machine class | Apple M1 MacBook Air |
| CPU | Apple M1 |
| Logical CPUs | 8 |
| Memory | 8 GiB |
| OS | macOS 26.5.1 |
| Kernel | Darwin 25.5.0 arm64 |
| CMake | 4.3.4 |
| Compiler | Apple clang 21.0.0 |
| Build type | Release |
| Benchmark framework | Google Benchmark v1.9.5 |
| Benchmark date | 2026-06-29 IST |
| Commit measured | `cd3164f2b5fe97b82937a4ee69776cd4d49e5434` |
| Command | `./scripts/run_benchmarks.sh` |

### High-level result summary

| Benchmark area | Summary |
|---|---|
| SPSC throughput | 19.7921M/s to 28.7810M/s depending on capacity and message count. |
| SPSC latency | Capacity 1024 produced much lower latency than capacity 65536. |
| Payload-size sweep | Larger payloads reached multi-GiB/s local memory throughput, with the highest reported byte throughput at 17.9730 GiB/s for 1024B payloads at capacity 1024 and 1,000,000 messages. |
| Broker end-to-end | In-memory broker reached up to 77.8142M/s; WAL-backed broker reached about 918k/s to 969k/s. |
| Batch publishing | Batch publishing was workload-sensitive and not universally faster. |
| Zero-copy SPSC | Zero-copy reserve/commit was competitive with regular try-emplace, but not faster for 64B payloads in this run. |
| Spin wait | Active spin primitives were very cheap; yielding/backoff was significantly more expensive. |

### Key interpretation

- The results support Aether-Stream as a strong local in-process messaging and queueing toolkit.
- Moderate SPSC capacities such as 1024 performed better than 65536 for throughput in the SPSC throughput benchmark.
- Large capacities can improve buffering, but they can increase queueing latency substantially.
- WAL durability has a large expected throughput cost compared with in-memory broker operation.
- Batch and zero-copy APIs are best presented as tuning and experimentation paths, not guaranteed speedups.
- Results are local synthetic measurements only.

### Caveats

- These results do not measure networking, distributed messaging, daemon behavior, or live cross-process service behavior.
- These results are not production guarantees and should not be generalized without rerunning the benchmarks on the target machine.
- The benchmark output included macOS CPU-frequency and affinity metadata warnings; those are preserved in the detailed results page.
- Benchmark smoke jobs are build/runtime checks only and should not be treated as published performance evidence.

## Links

- [Benchmark methodology](benchmark-methodology.md)
- [Detailed M1 MacBook Air benchmark run](benchmark-results/m1-macbook-air-2026-06-29.md)
- [README](../README.md)
