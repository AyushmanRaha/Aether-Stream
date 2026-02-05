#  Aether-Stream

> **Ultra-Low Latency Lock-Free Messaging Engine**

---

### ⚠️ CAUTION: UNDER ACTIVE DEVELOPMENT ⚠️
> **This project is currently in the MVP (Minimum Viable Product) phase.** > While the core lock-free logic and persistence layer are functional, the API is subject to breaking changes as we move toward the v1.0 "Elite Tier" release. 
> *Not yet recommended for production use.*

---

##  Overview
**Aether-Stream** is a high-performance C++20 middleware designed for sub-microsecond latency environments (High-Frequency Trading, Real-Time Analytics). 

Unlike standard message queues that rely on `std::mutex` and OS-level locking, Aether-Stream implements a **Lock-Free Single Producer Single Consumer (SPSC)** architecture. It utilizes **C++20 Atomics** with explicit memory ordering and **Memory-Mapped Files (mmap)** for zero-copy persistence, effectively bypassing standard OS bottlenecks.

##  Key Features (MVP)
* **Lock-Free SPSC Ring Buffer:** Wait-free communication between threads using `std::atomic` and Acquire/Release semantics.
* **Hardware Aware:** Cache-line aligned data structures (`alignas(64)`) to prevent False Sharing and CPU cache thrashing.
* **Zero-Copy Persistence:** Write-Ahead Log (WAL) implemented via `mmap` for direct memory-to-disk journaling.
* **Modern C++20:** Uses Concepts (`requires`) and smart pointers for type safety.

##  Tech Stack
* **Language:** C++20
* **Build System:** CMake (3.20+)
* **Testing:** Google Test (GTest) - *Fetched automatically via CMake*
* **Platform:** Linux / macOS (POSIX Compliant)

##  Getting Started

### Prerequisites
* C++ Compiler supporting C++20 (GCC 10+, Clang 12+, MSVC 19.28+)
* CMake 3.20 or higher
