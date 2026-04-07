FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DMAKROUNADB_ENABLE_SANITIZERS=OFF \
    -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc -static-libstdc++" \
    && cmake --build build --target makrounadb_server

FROM scratch

COPY --from=builder /src/build/makrounaDB /usr/local/bin/makrounaDB

EXPOSE 6379
VOLUME ["/data"]

ENTRYPOINT ["/usr/local/bin/makrounaDB"]
CMD ["--port", "6379", "--aof", "/data/appendonly.aof"]
