# M1 MacBook Air benchmark run — 2026-06-29

## Table of contents

- [Redaction policy](#redaction-policy)
- [Run metadata](#run-metadata)
- [Benchmark context metadata](#benchmark-context-metadata)
- [Benchmark warnings](#benchmark-warnings)
- [SPSC throughput](#spsc-throughput)
- [SPSC latency](#spsc-latency)
- [Payload-size sweep](#payload-size-sweep)
- [Broker end-to-end](#broker-end-to-end)
- [Batch publish](#batch-publish)
- [Zero-copy SPSC](#zero-copy-spsc)
- [Spin wait](#spin-wait)
- [Interpretation](#interpretation)
- [Caveats](#caveats)
- [Links](#links)

## Redaction policy

The original benchmark outputs included local-only metadata such as the machine hostname and absolute executable paths. Those fields were redacted before publication.

| Original category | Published form |
|---|---|
| Local hostname | `<hostname-redacted>` |
| Absolute repository path | `<repo>` |
| Local result directory | `<repo>/benchmark-results/20260629-044501` |
| Local untracked Git status | Omitted |

Benchmark values, counters, timestamps, build type, compiler version, OS version, CPU class, and memory size were not modified.

## Run metadata

| Field | Value |
|---|---|
| Result directory | `<repo>/benchmark-results/20260629-044501` |
| Timestamp UTC | 2026-06-28T23:15:02Z |
| Benchmark date | 2026-06-29 IST |
| Commit | `cd3164f2b5fe97b82937a4ee69776cd4d49e5434` |
| Machine class | Apple M1 MacBook Air |
| CPU | Apple M1 |
| Logical CPUs | 8 |
| Memory | 8 GiB |
| Kernel | Darwin 25.5.0 arm64 |
| macOS | 26.5.1, build 25F80 |
| CMake | 4.3.4 |
| Compiler | Apple clang 21.0.0 |
| Target | arm64-apple-darwin25.5.0 |
| Thread model | POSIX |
| Build type | Release |
| Benchmark framework | Google Benchmark v1.9.5 |
| Command | `./scripts/run_benchmarks.sh` |

## Benchmark context metadata

| Field | Value |
|---|---|
| CPUs reported by benchmark framework | 8 |
| Reported MHz per CPU | 24 |
| CPU scaling enabled | false |
| L1 data cache | 64 KiB |
| L1 instruction cache | 128 KiB |
| L2 unified cache | 4096 KiB x 8 / 4 MiB shared metadata as reported |
| JSON schema version | 1 |

The `mhz_per_cpu=24` value is a benchmark metadata artifact caused by macOS frequency reporting limitations, not a useful CPU-speed claim.

## Benchmark warnings

The benchmark text output reported:

- `Unable to determine clock rate from sysctl: hw.cpufrequency: No such file or directory`
- `This does not affect benchmark measurements, only the metadata output.`
- `***WARNING*** Failed to set thread affinity. Estimated CPU frequency may be incorrect.`

These warnings are expected on this macOS run and should be treated as platform metadata limitations. They do not invalidate the benchmark timing values, but they should be kept visible for honesty.

## SPSC throughput

Source executable after redaction: `<repo>/build/release/benchmarks/bench_spsc_throughput`

Timestamp: `2026-06-29T04:45:03+05:30`

Load average: `2.44, 3.13, 3.20`

| Benchmark                                         |          Time |       CPU | Iterations | Capacity | Consumer retries | Producer retries |  Items/sec | Messages/sec |
| ------------------------------------------------- | ------------: | --------: | ---------: | -------: | ---------------: | ---------------: | ---------: | -----------: |
| `benchmark_throughput<64>/100000/manual_time`     |  3,586,115 ns | 25,636 ns |        794 |       64 |         440.649M |         109.248k | 27.8853M/s |   27.8853M/s |
| `benchmark_throughput<64>/1000000/manual_time`    | 37,880,729 ns | 32,857 ns |         77 |       64 |         419.351M |         448.396k | 26.3986M/s |   26.3986M/s |
| `benchmark_throughput<256>/100000/manual_time`    |  3,681,671 ns | 27,031 ns |        780 |      256 |         438.444M |         321.357k | 27.1616M/s |   27.1616M/s |
| `benchmark_throughput<256>/1000000/manual_time`   | 35,354,428 ns | 30,481 ns |         79 |      256 |         449.296M |         102.322k | 28.2850M/s |   28.2850M/s |
| `benchmark_throughput<1024>/100000/manual_time`   |  3,603,415 ns | 23,892 ns |        731 |   1.024k |         395.424M |          94.484k | 27.7515M/s |   27.7515M/s |
| `benchmark_throughput<1024>/1000000/manual_time`  | 34,745,117 ns | 26,012 ns |         83 |   1.024k |         455.985M |              752 | 28.7810M/s |   28.7810M/s |
| `benchmark_throughput<65536>/100000/manual_time`  |  5,047,102 ns | 31,122 ns |        547 |  65.536k |         731.642M |           7.972k | 19.8133M/s |   19.8133M/s |
| `benchmark_throughput<65536>/1000000/manual_time` | 50,525,234 ns | 41,305 ns |         59 |  65.536k |         770.184M |                0 | 19.7921M/s |   19.7921M/s |

## SPSC latency

Source executable after redaction: `<repo>/build/release/benchmarks/bench_spsc_latency`

Timestamp: `2026-06-29T04:45:30+05:30`

Load average: `2.87, 3.18, 3.22`

| Benchmark                                     |          Time |          CPU | Iterations | Capacity |  Items/sec |        Min |         P50 |         P95 |         P99 |       P99.9 |         Max | Consumer retries | Producer retries |
| --------------------------------------------- | ------------: | -----------: | ---------: | -------: | ---------: | ---------: | ----------: | ----------: | ----------: | ----------: | ----------: | ---------------: | ---------------: |
| `benchmark_latency<1024>/100000/manual_time`  |  5,179,791 ns | 1,107,797 ns |        552 |   1.024k | 19.3058M/s |   14.5k ns |  50.708k ns |  54.209k ns |  54.708k ns |  55.041k ns |  55.125k ns |         30.6552M |          120.44k |
| `benchmark_latency<1024>/200000/manual_time`  | 10,018,016 ns | 2,038,640 ns |        278 |   1.024k | 19.9640M/s |   11.5k ns |   47.25k ns |  53.542k ns |  54.792k ns |  55.375k ns |  55.584k ns |         21.7825M |          73.777k |
| `benchmark_latency<65536>/100000/manual_time` |  4,082,140 ns | 1,605,629 ns |        685 |  65.536k | 24.4970M/s | 15.917k ns | 1.06504M ns | 1.41092M ns | 1.44037M ns | 1.44637M ns | 1.44712M ns |                0 |                0 |
| `benchmark_latency<65536>/200000/manual_time` |  8,308,406 ns | 3,710,834 ns |        337 |  65.536k | 24.0720M/s | 15.333k ns |  1.9665M ns | 2.71937M ns | 2.77025M ns | 2.78012M ns | 2.78133M ns |         183.605k |          43.561k |

## Payload-size sweep

Source executable after redaction: `<repo>/build/release/benchmarks/bench_payload_sizes`

Timestamp: `2026-06-29T04:45:50+05:30`

Load average: `2.82, 3.15, 3.21`

| Benchmark                                                 |          Time |        CPU | Iterations | Payload | Capacity |  Items/sec |     Bytes/sec | Consumer retries | Producer retries |
| --------------------------------------------------------- | ------------: | ---------: | ---------: | ------: | -------: | ---------: | ------------: | ---------------: | ---------------: |
| `benchmark_payload_size<8, 1024>/100000/manual_time`      |  4,135,268 ns |  26,471 ns |        680 |      8B |   1.024k | 24.1822M/s | 184.496 MiB/s |         502.746M |           1.695k |
| `benchmark_payload_size<8, 1024>/1000000/manual_time`     | 40,659,496 ns |  29,897 ns |         68 |      8B |   1.024k | 24.5945M/s | 187.641 MiB/s |         471.110M |          61.948k |
| `benchmark_payload_size<32, 1024>/100000/manual_time`     |  5,389,531 ns |  23,858 ns |        515 |     32B |   1.024k | 18.5545M/s | 566.238 MiB/s |         482.311M |              774 |
| `benchmark_payload_size<32, 1024>/1000000/manual_time`    | 44,572,324 ns |  25,536 ns |         56 |     32B |   1.024k | 22.4354M/s | 684.676 MiB/s |         457.696M |              430 |
| `benchmark_payload_size<64, 1024>/100000/manual_time`     |  4,231,457 ns |  32,614 ns |        658 |     64B |   1.024k | 23.6325M/s | 1.40861 GiB/s |         270.911M |          13.599k |
| `benchmark_payload_size<64, 1024>/1000000/manual_time`    | 42,689,581 ns |  32,448 ns |         67 |     64B |   1.024k | 23.4249M/s | 1.39623 GiB/s |         301.891M |           5.801k |
| `benchmark_payload_size<256, 1024>/100000/manual_time`    |  4,155,514 ns |  30,257 ns |        676 |    256B |   1.024k | 24.0644M/s |  5.7374 GiB/s |         131.813M |           10.05k |
| `benchmark_payload_size<256, 1024>/1000000/manual_time`   | 41,407,562 ns |  32,552 ns |         67 |    256B |   1.024k | 24.1502M/s | 5.75785 GiB/s |         136.970M |           4.478k |
| `benchmark_payload_size<1024, 1024>/100000/manual_time`   |  5,330,414 ns |  36,200 ns |        524 |   1024B |   1.024k | 18.7603M/s | 17.8912 GiB/s |         711.722M |           1.652k |
| `benchmark_payload_size<1024, 1024>/1000000/manual_time`  | 53,061,521 ns |  36,396 ns |         53 |   1024B |   1.024k | 18.8460M/s | 17.9730 GiB/s |         724.022M |           2.214k |
| `benchmark_payload_size<8, 65536>/100000/manual_time`     |  5,379,581 ns |  26,216 ns |        518 |      8B |  65.536k | 18.5888M/s | 141.821 MiB/s |         838.968M |                0 |
| `benchmark_payload_size<8, 65536>/1000000/manual_time`    | 51,845,609 ns |  26,889 ns |         54 |      8B |  65.536k | 19.2880M/s | 147.156 MiB/s |         802.875M |                0 |
| `benchmark_payload_size<32, 65536>/100000/manual_time`    |  6,146,770 ns |  42,525 ns |        446 |     32B |  65.536k | 16.2687M/s | 496.482 MiB/s |         423.267M |                0 |
| `benchmark_payload_size<32, 65536>/1000000/manual_time`   | 60,933,526 ns |  46,061 ns |         49 |     32B |  65.536k | 16.4113M/s | 500.834 MiB/s |         471.717M |          55.641k |
| `benchmark_payload_size<64, 65536>/100000/manual_time`    |  4,190,554 ns |  58,481 ns |        669 |     64B |  65.536k | 23.8632M/s | 1.42236 GiB/s |         254.684M |                0 |
| `benchmark_payload_size<64, 65536>/1000000/manual_time`   | 42,181,688 ns |  60,909 ns |         66 |     64B |  65.536k | 23.7070M/s | 1.41305 GiB/s |         268.827M |                0 |
| `benchmark_payload_size<256, 65536>/100000/manual_time`   |  4,892,462 ns | 184,776 ns |        577 |    256B |  65.536k | 20.4396M/s | 4.87318 GiB/s |         275.437M |                0 |
| `benchmark_payload_size<256, 65536>/1000000/manual_time`  | 46,893,926 ns | 193,717 ns |         60 |    256B |  65.536k | 21.3247M/s | 5.08421 GiB/s |         264.569M |                0 |
| `benchmark_payload_size<1024, 65536>/100000/manual_time`  |  6,477,079 ns | 971,333 ns |        439 |   1024B |  65.536k | 15.4391M/s | 14.7238 GiB/s |         573.238M |                0 |
| `benchmark_payload_size<1024, 65536>/1000000/manual_time` | 55,323,748 ns | 973,818 ns |         44 |   1024B |  65.536k | 18.0754M/s | 17.2381 GiB/s |         586.230M |                0 |

## Broker end-to-end

Source executable after redaction: `<repo>/build/release/benchmarks/bench_broker_end_to_end`

Timestamp: `2026-06-29T04:46:58+05:30`

Load average: `2.80, 3.07, 3.17`

| Benchmark                              |         Time |          CPU | Iterations |  Items/sec | Published | Consumed | WAL records | WAL bytes |
| -------------------------------------- | -----------: | -----------: | ---------: | ---------: | --------: | -------: | ----------: | --------: |
| `BM_BrokerEndToEnd_InMemory_64B_1024`  |    13,164 ns |    13,160 ns |    214,370 | 77.8142M/s |  219.515M | 219.515M |           — |         — |
| `BM_BrokerEndToEnd_InMemory_64B_65536` |    73,145 ns |    73,144 ns |     37,705 | 13.9998M/s |  38.6099M | 38.6099M |           — |         — |
| `BM_BrokerEndToEnd_Wal_64B_1024`       | 1,117,981 ns | 1,056,975 ns |      2,669 | 968.803k/s |  2.73306M | 2.73306M |    2.73306M |  284.238M |
| `BM_BrokerEndToEnd_Wal_64B_65536`      | 1,176,506 ns | 1,115,297 ns |      2,519 | 918.141k/s |  2.57946M | 2.57946M |    2.57946M |  268.263M |

## Batch publish

Source executable after redaction: `<repo>/build/release/benchmarks/bench_batch_publish`

Timestamp: `2026-06-29T04:47:15+05:30`

Load average: `2.72, 3.04, 3.15`

| Benchmark                          |      Time |       CPU | Iterations |  Items/sec |
| ---------------------------------- | --------: | --------: | ---------: | ---------: |
| `BM_BrokerSingle_64B_1024`         | 12,961 ns | 12,961 ns |    216,254 | 79.0060M/s |
| `BM_BatchBroker_Batch8_64B_1024`   | 18,075 ns | 18,075 ns |    153,826 | 56.6524M/s |
| `BM_BatchBroker_Batch32_64B_1024`  | 19,262 ns | 19,262 ns |    144,719 | 53.1611M/s |
| `BM_BrokerSingle_64B_65536`        | 80,715 ns | 80,715 ns |     36,317 | 12.6866M/s |
| `BM_BatchBroker_Batch8_64B_65536`  | 77,831 ns | 77,830 ns |     35,035 | 13.1568M/s |
| `BM_BatchBroker_Batch32_64B_65536` | 82,759 ns | 82,759 ns |     34,529 | 12.3733M/s |

## Zero-copy SPSC

Source executable after redaction: `<repo>/build/release/benchmarks/bench_zero_copy_spsc`

Timestamp: `2026-06-29T04:47:41+05:30`

Load average: `2.38, 2.93, 3.11`

| Benchmark                            |    Time |     CPU |  Iterations |  Items/sec |
| ------------------------------------ | ------: | ------: | ----------: | ---------: |
| `BM_SpscTryEmplace_64B_1024`         | 12.0 ns | 12.0 ns | 234,549,079 | 83.2732M/s |
| `BM_ZeroCopyReserveCommit_64B_1024`  | 12.2 ns | 12.2 ns | 227,117,322 | 82.1498M/s |
| `BM_SpscTryEmplace_64B_65536`        | 12.2 ns | 12.2 ns | 229,606,815 | 81.9356M/s |
| `BM_ZeroCopyReserveCommit_64B_65536` | 12.9 ns | 12.9 ns | 216,230,075 | 77.2867M/s |

## Spin wait

Source executable after redaction: `<repo>/build/release/benchmarks/bench_spin_wait`

Timestamp: `2026-06-29T04:47:59+05:30`

Load average: `2.37, 2.90, 3.10`

| Benchmark            |     Time |      CPU |    Iterations |
| -------------------- | -------: | -------: | ------------: |
| `BM_CpuRelax`        | 0.313 ns | 0.313 ns | 8,963,642,824 |
| `BM_ThreadYield`     |   131 ns |   131 ns |    21,312,484 |
| `BM_SpinWaitPause`   | 0.318 ns | 0.318 ns | 8,789,056,369 |
| `BM_SpinWaitBackoff` |   556 ns |   543 ns |     5,337,776 |

## Interpretation

- The run supports Aether-Stream as a strong local in-process messaging and queueing toolkit, within its documented SPSC and local-first scope.
- Moderate SPSC capacities such as 1024 performed better than 65536 for throughput in the SPSC throughput benchmark.
- Larger capacities can provide more buffering, but the latency benchmark shows they can also increase queueing latency substantially.
- WAL-backed broker operation has a large expected throughput cost compared with in-memory broker operation because each publish includes durability work before queue publication.
- Batch publishing and zero-copy reserve/commit are useful tuning and experimentation paths, not guaranteed speedups across payloads, capacities, or machines.
- Active spin primitives measured very cheaply in this run, while yielding/backoff paths were significantly more expensive.

## Caveats

- These are local synthetic measurements from one redacted Apple M1 MacBook Air run.
- The results do not measure networking, distributed messaging, daemon behavior, or live cross-process service behavior.
- The results are not production guarantees and should not be treated as latency or throughput promises for other hardware, operating systems, compiler versions, thermal states, or workloads.
- The benchmark framework reported macOS CPU-frequency and affinity metadata warnings; they are preserved above for honesty.
- Benchmark smoke checks remain build/runtime checks only and are not publishable performance evidence.

## Links

- [Performance results](../performance-results.md)
- [Benchmark methodology](../benchmark-methodology.md)
- [README](../../README.md)
