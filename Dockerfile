# Build Stage
FROM ubuntu:22.04 AS builder

# Install dependencies (prometheus-cpp might require compiling from source depending on the ubuntu version, but for simplicity we assume it's in the package manager or we build without it if not found by CMake)
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    cmake \
    git \
    libprometheus-cpp-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Run Stage
FROM ubuntu:22.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    libprometheus-cpp-pull-dev \
    libprometheus-cpp-core-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/mercury /app/mercury
RUN mkdir snapshots

EXPOSE 8080

CMD ["./mercury"]
