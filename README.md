# makrounaDB

Redis-inspired in-memory database written in modern C++.

`makrounaDB` runs as a CLI server, supports RESP2-style requests, TTL, and AOF persistence.

## 1) Prerequisites

Install required tools:

```bash
sudo apt update
sudo apt install -y g++ cmake ninja-build python3 netcat-openbsd
```

> `netcat-openbsd` is only needed for manual socket testing.

## 2) Build the project

From the repository root:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

This produces the server binary:

```bash
./build/makrounaDB
```

## 3) Run test suite

```bash
ctest --test-dir build --output-on-failure
```

## 4) Start the server

### Basic run

```bash
./build/makrounaDB
```

### Run with explicit options

```bash
./build/makrounaDB --port 6379 --aof data/appendonly.aof
```

### CLI help

```bash
./build/makrounaDB --help
```

## 5) Natural language CLI mode

You can interact with a running server using natural language:

```bash
./build/makrounaDB --nl-shell --host 127.0.0.1 --port 6379
```

Example natural language inputs:
- `ping`
- `set user to ghassen`
- `what is user?`
- `increment counter`
- `expire user in 30 seconds`
- `ttl user`
- `does user exist?`
- `delete user`

Type `quit` to exit NL shell mode.

## 6) Test commands manually (socket level)

Use the same protocol style documented in `docs/protocol/command-spec.md`.

RESP request format:

`*<argc>\r\n$<len>\r\n<arg>\r\n...`

Reply types:
- Simple string: `+...\r\n`
- Error: `-ERR ...\r\n`
- Integer: `:<n>\r\n`
- Bulk string: `$<len>\r\n...\r\n`
- Null bulk string: `$-1\r\n`

Open a second terminal and send RESP commands using `nc -N`:

```bash
printf '*1\r\n$4\r\nPING\r\n' | nc -N 127.0.0.1 6379
printf '*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$10\r\nmakrounaDB\r\n' | nc -N 127.0.0.1 6379
printf '*2\r\n$3\r\nGET\r\n$4\r\nname\r\n' | nc -N 127.0.0.1 6379
printf '*2\r\n$4\r\nINCR\r\n$7\r\ncounter\r\n' | nc -N 127.0.0.1 6379
printf '*3\r\n$6\r\nEXPIRE\r\n$4\r\nname\r\n$2\r\n10\r\n' | nc -N 127.0.0.1 6379
printf '*2\r\n$3\r\nTTL\r\n$4\r\nname\r\n' | nc -N 127.0.0.1 6379
```

Supported server commands:
- `PING [message]`
- `ECHO message`
- `SET key value`
- `GET key`
- `DEL key [key ...]`
- `EXISTS key [key ...]`
- `INCR key`
- `EXPIRE key seconds`
- `TTL key`

## 7) Test commands with example client

With server running:

```bash
python3 examples/python_client.py
```

## 8) Useful project scripts

```bash
bash scripts/dev/build.sh
bash scripts/dev/test.sh
bash scripts/dev/run.sh 6379 data/appendonly.aof
bash scripts/dev/bench.sh --host 127.0.0.1 --port 6379 --requests 10000
bash scripts/ci/run_ci.sh
```

## 9) Benchmarks

Run:

```bash
python3 benchmarks/simple_benchmark.py --host 127.0.0.1 --port 6379 --requests 5000
```

Report is generated at:

```bash
benchmarks/report.md
```

## 10) Export release artifacts (.deb / .rpm / Windows .zip / all)

```bash
bash scripts/release/build_release.sh --format all
bash scripts/release/build_release.sh --format deb
bash scripts/release/build_release.sh --format rpm
bash scripts/release/build_release.sh --format windows
```

Artifacts are exported to `./releases`.

Optional GitHub Release upload (requires `gh auth login`):

```bash
bash scripts/release/build_release.sh --format all --github-release --tag v0.1.0
```

## 11) Documentation map

- `docs/README.md`
- `docs/architecture/overview.md`
- `docs/protocol/command-spec.md`
- `docs/meeting-notes/template.md`
