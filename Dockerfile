# ---- build stage ----
FROM gcc:14 AS builder

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    ninja-build \
    libhpdf-dev \
    libpugixml-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
RUN cmake --build build --target docraft_tool --config Release -j "$(nproc)"

# ---- runtime stage ----
FROM debian:trixie-slim
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    libhpdf-2.3.0 \
    libpugixml1v5 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/build/artifacts/bin/docraft_tool /usr/local/bin/docraft_tool

ENTRYPOINT ["/usr/local/bin/docraft_tool"]