# Mercury

Mercury is a high-performance, low-latency trade execution engine written in C++17. 

## Features
* **Low Latency**: Processes concurrent market orders through price-time priority queues, designed to sustain 1M+ transactions/sec.
* **O(1) Data Structures**: Optimized order routing and execution using hash maps, priority queues, and custom memory pools to minimize allocation overhead.
* **Fault Tolerance**: Implements fault-tolerant recovery with append-only event logs and state snapshots.
* **Instrumentation**: Instruments latency and throughput with Prometheus.
* **CI/CD**: Automates tests and Docker builds through GitHub Actions.

## Building and Running

### Prerequisites
* CMake 3.14+
* C++17 compatible compiler
* Prometheus-cpp (optional, for metrics)

### Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Run
```bash
./mercury
```
