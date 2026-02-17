# ApexKey: High-Performance Persistent KV Store

**ApexKey** is a distributed, thread-safe Key-Value storage engine built with **C++20** and **gRPC**. It implements a full CRUD lifecycle and features a Write-Ahead Log (WAL) style persistence layer for data durability.



## 🚀 Performance Metrics
Validated through a custom benchmarking suite on macOS (ARM64):
* **Throughput:** 3,046.51 operations per second (Sequential PUT)
* **Average Latency:** 0.328 ms per operation
* **Reliability:** 100% data durability verified across server restarts via WAL persistence

## ✨ Key Features
* **gRPC Framework:** Leverages Protocol Buffers for structured, efficient cross-process communication.
* **Thread-Safety:** Utilizes `std::mutex` and RAII-style locking to ensure memory safety under concurrent load.
* **Data Persistence:** Implements a Write-Ahead Log (WAL) strategy to sync in-memory `std::unordered_map` state to disk.
* **Modern C++:** Built using **C++20** standards and managed with the **vcpkg** package manager.



## 🛠️ Tech Stack
* **Language:** C++20
* **RPC Framework:** gRPC
* **Serialization:** Protocol Buffers (protobuf)
* **Build System:** CMake
* **Dependency Management:** vcpkg

## ⚡ Quick Start

### Prerequisites
* CMake 3.15+
* vcpkg (with `grpc` and `protobuf` installed)

### Build
```bash
# Configure the project with vcpkg toolchain
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path_to_vcpkg]/scripts/buildsystems/vcpkg.cmake

# Build the binaries
cmake --build build
``` 

### Run

1. Start the server

```bash
./build/ApexKeyServer
```

2. Run Client & Benchmark:

```bash
./build/ApexKeyClient
```

