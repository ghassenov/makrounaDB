# makrounaDB — Technical Presentation

## Building a Redis-Inspired Database in Modern C++

Author: makrounaDB team  
Date: 2026-04-07

---

## 0) Presentation Roadmap

This presentation follows the same order for every cornerstone of the project:

1. **Concept (deep technical explanation)**
2. **Implementation (how we built it in makrounaDB)**

We cover:
- Project vision and architecture
- Build system and modular design
- Network server runtime
- RESP protocol layer
- Command execution engine
- In-memory storage model
- TTL and expiration lifecycle
- AOF persistence and recovery
- CLI and natural-language shell
- Testing strategy (unit + integration + socket-level)
- Benchmarking and tooling
- Repository hygiene and operational readiness
- Lessons learned and next technical steps

---

## 1) System Vision and Architecture

### 1.1 Concept

A Redis-like system is fundamentally a **low-latency, in-memory, networked data service**. Unlike traditional disk-first database engines, in-memory servers optimize for rapid read/write paths, predictable command semantics, and efficient wire-level protocol handling.

At a systems level, this type of database has several architectural pillars:

- **State model**: key-value objects held in memory.
- **Protocol model**: deterministic request/response framing over TCP.
- **Execution model**: parse request → dispatch command → mutate/query state → encode response.
- **Durability model**: optional persistence to recover state after restart.
- **Time model**: support for expirations and TTL semantics.
- **Operational model**: testability, reproducibility, and observability via scripts/docs.

Redis-style servers are successful because they combine:
- simple command primitives,
- strict protocol behavior,
- high request throughput,
- and highly predictable response contracts.

makrounaDB intentionally mirrors that philosophy for an educational but production-minded implementation.

### 1.2 How we implemented it

We structured the system as explicit modules:

- `src/network` + `include/network`: TCP server/event loop lifecycle
- `src/protocol` + `include/protocol`: RESP request parsing + reply encoding
- `src/command` + `include/command`: command dispatcher and handlers
- `src/storage` + `include/storage`: in-memory keyspace + TTL metadata
- `src/persistence` + `include/persistence`: append-only file persistence
- `src/app`: CLI entrypoint and runtime orchestration
- `src/common` + `include/common`: shared utilities, natural-language parser

The request path is:

**Socket bytes → request parser → command dispatcher → storage/persistence → RESP reply bytes**

This gives us clean separation of concerns and test boundaries.

---

## 2) Build System and Modular Compilation

### 2.1 Concept

A robust build system is a cornerstone for C++ systems because it controls:
- dependency graph resolution,
- target boundaries,
- compiler options,
- sanitizer usage,
- test registration.

A clean build graph reduces coupling and allows independent testing of subsystems. For systems software, this is critical for velocity and correctness.

### 2.2 How we implemented it

We adopted CMake with a core library + executable strategy:

- `makrounadb_core` static target containing command/protocol/storage/persistence/common modules
- `makrounaDB` executable target (server CLI)
- dedicated unit and integration test targets via CTest

Key choices:
- C++20 standard
- sanitizer support (`ASan` + `UBSan`) via CMake options
- test registration (`add_test`) for automated CI and local execution

This setup gave us deterministic build and test commands:
- configure + build
- execute all unit/integration tests

---

## 3) Network Runtime (TCP Server)

### 3.1 Concept

A database server is fundamentally an event-driven network application. The key responsibilities of the network layer are:

- create/listen on TCP socket,
- accept incoming client connections,
- read request bytes correctly (partial reads included),
- maintain per-client input buffers,
- write responses reliably (partial writes handled),
- detect disconnects and cleanup descriptors.

Because TCP is a stream protocol, boundaries are not preserved. Request framing must be reconstructed above TCP.

### 3.2 How we implemented it

In `network::Server`:

- Listener socket created with `socket`, `bind`, `listen`
- `SO_REUSEADDR` applied for smoother restarts
- Main loop uses `select`
- Each connected client has a dedicated request buffer
- Incoming bytes are appended to that buffer
- Parser is called repeatedly until input is incomplete
- Responses are sent with a `send_all` helper for full payload delivery
- Disconnected clients are closed and removed from tracking structures

Lifecycle handling:
- `run()` starts listener and event loop
- `stop()` triggers graceful shutdown
- signal handlers in main (`SIGINT`, `SIGTERM`) stop server cleanly

This is a practical, deterministic event-loop baseline appropriate for educational Redis-like architecture.

---

## 4) RESP Protocol: Framing, Parsing, and Reply Encoding

### 4.1 Concept

RESP (Redis Serialization Protocol) is a simple, high-performance text/binary hybrid protocol with strict framing rules.

Why RESP matters:
- Language-agnostic client interoperability
- deterministic parsing
- compact enough for high-throughput command traffic

Core ideas:
- Requests commonly encoded as arrays of bulk strings
- Replies have explicit types: simple string, error, integer, bulk, null bulk
- CRLF (`\r\n`) is a framing delimiter

A robust parser must support:
- incremental parsing from partial network buffers,
- explicit error states for malformed input,
- incomplete states (not enough bytes yet) without disconnecting prematurely.

### 4.2 How we implemented it

Two protocol components:

1. **Request parser** (`protocol/request_parser`)
   - Supports RESP arrays (`*...` + `$...`) and inline fallback
   - Returns structured parse result:
     - status (`complete`, `incomplete`, `error`)
     - parsed arguments
     - bytes consumed
     - error message

2. **Reply encoder** (`protocol/resp`)
   - Helpers for all response types:
     - `simple_string`
     - `bulk_string`
     - `null_bulk_string`
     - `integer`
     - `error`

Parser design benefits:
- safe integration with stream-oriented socket input
- precise error responses to malformed requests
- repeat parsing loop until buffered command stream is consumed

---

## 5) Command Dispatcher and Execution Semantics

### 5.1 Concept

In Redis-like systems, the dispatcher is the semantic gatekeeper. It translates parsed tokens into behavior and enforces:
- arity validation,
- command normalization (case-insensitive names),
- response type correctness,
- consistent error messaging.

A command engine should be deterministic: same input always produces same output against same state.

### 5.2 How we implemented it

`CommandDispatcher` in `src/command`:

- command keyword normalized to uppercase
- command handlers implemented for:
  - `PING`, `ECHO`
  - `SET`, `GET`
  - `DEL`, `EXISTS`
  - `INCR`
  - `EXPIRE`, `TTL`
- argument count checks for each command path
- typed RESP replies returned directly from handlers
- exception-safe handling for numeric conversion paths (`INCR`, `EXPIRE`)

Additional hook:
- `run_maintenance()` calls storage expiration cleanup to support periodic lifecycle work from the network loop.

---

## 6) In-Memory Storage Engine

### 6.1 Concept

The key-value store is the data plane of the server. It must provide low-latency operations with clear semantics:
- set value
- get value
- delete key
- existence check
- numeric mutation (`INCR`)

For MVP, `std::unordered_map` is an effective hash table baseline with expected O(1)-average operations.

### 6.2 How we implemented it

`KeyValueStore` provides:
- `set`
- `get`
- `del`
- `exists`
- `incr`

Implementation details:
- values stored as strings
- `INCR` converts string to integer and validates full parse consumption
- non-integer values trigger error path handled by dispatcher
- key operations are integrated with expiration logic (lazy invalidation)

This gives a robust and testable storage abstraction ready for future data-structure upgrades.

---

## 7) TTL and Expiration Lifecycle

### 7.1 Concept

TTL in Redis-like systems combines two techniques:

1. **Lazy expiration**
   - key is checked at access time
   - expired key is removed just-in-time

2. **Active/periodic expiration**
   - background/periodic sweep removes expired keys proactively

Why both are needed:
- lazy keeps command path correct,
- periodic prevents stale memory accumulation when keys are not accessed.

Semantics:
- `TTL` must distinguish missing key, no expire, and remaining lifetime.
- `EXPIRE` sets expiration relative to now.

### 7.2 How we implemented it

In storage:
- `keyspace_` for values
- `expirations_` map for expiration timestamps (`steady_clock::time_point`)

Key methods:
- `expire(key, seconds)`
- `ttl(key)` returning `-2`, `-1`, or remaining seconds
- `cleanup_expired(max_to_remove)` for bounded periodic sweep

Lazy enforcement:
- `get` and `exists` call `erase_if_expired` before returning

Periodic enforcement:
- server event loop invokes `dispatcher.run_maintenance()` each tick

This reproduces expected TTL behavior while keeping runtime implementation straightforward.

---

## 8) AOF Persistence and Crash Recovery

### 8.1 Concept

In-memory systems need a durability strategy. AOF (Append-Only File) logs mutating commands so state can be reconstructed after restart.

AOF advantages:
- replay is intuitive,
- command log is transparent,
- incremental durability without full snapshots.

Core design concerns:
- only successful mutating commands should be appended,
- replay must use same command semantics as live traffic,
- flush policy balances durability and performance.

### 8.2 How we implemented it

`AofPersistence` supports:
- opening append stream (`open`)
- replaying existing log into dispatcher (`load_and_replay`)
- appending successful write commands (`append_if_write`)
- periodic flush (`periodic_sync`) with `everysec` cadence
- final flush on shutdown (`flush`)

Write-command whitelist:
- `SET`, `DEL`, `INCR`, `EXPIRE`

Replay flow:
- read full AOF file
- parse with same request parser used for sockets
- execute through same dispatcher

Runtime integration:
- startup does replay before listener loop
- event loop appends writes and triggers periodic sync

This gives durable recovery with minimal conceptual complexity.

---

## 9) CLI Tooling and UX Layer

### 9.1 Concept

A database server is more usable when packaged as a discoverable CLI:
- explicit flags for port/path behavior
- help output for operators
- consistent startup logs
- optional branding for demo/user experience

CLI ergonomics reduce setup friction and support reproducible invocation patterns.

### 9.2 How we implemented it

`main.cpp` now supports:
- `--help`
- `--port <port>`
- `--aof <path>`
- `--no-banner`

Branding:
- executable output name: `makrounaDB`
- ASCII startup banner including pasta plate art

Operational result:
- straightforward local startup and script automation
- clear runtime visibility (port + AOF path)

---

## 10) Natural-Language Shell Mode

### 10.1 Concept

Natural-language interaction is an application-layer usability feature that translates human intent to deterministic command calls.

Important principle:
- NL interface should map to strict backend commands (never bypass protocol/semantics).

This creates a “friendly frontend” while preserving the same server execution path.

### 10.2 How we implemented it

Two pieces:

1. **NL parser** (`common/nl_parser`)
   - regex-driven phrase mapping, e.g.:
     - “set user to ghassen” → `SET user ghassen`
     - “what is user?” → `GET user`
     - “increment counter” → `INCR counter`
     - “expire user in 30 seconds” → `EXPIRE user 30`

2. **NL shell mode** (`--nl-shell` in CLI)
   - connects to running server over TCP
   - encodes mapped command into RESP
   - sends request and reads RESP replies
   - prints user-friendly formatted output

This preserved strict server protocol while introducing higher-level UX.

---

## 11) Testing Strategy and Quality Gates

### 11.1 Concept

Database infrastructure requires layered testing:
- **unit tests** for semantic and parsing correctness,
- **integration tests** for cross-module command behavior,
- **socket-level tests** for real transport correctness.

This reduces blind spots and catches regressions early.

### 11.2 How we implemented it

Unit tests include:
- key-value behavior and TTL logic
- command dispatcher correctness
- request parser edge cases
- AOF append/replay
- NL parser phrase mapping

Integration tests include:
- command flow across storage/dispatcher
- socket-level end-to-end test:
  - launches actual server process
  - sends RESP over TCP
  - validates responses
  - restarts server and validates AOF recovery

All tests are registered with CTest and pass in full suite runs.

---

## 12) Benchmarking and Performance Visibility

### 12.1 Concept

Performance work should begin with repeatable baselines:
- throughput (ops/sec)
- latency distribution (p50, p95)

Even a simple benchmark harness is valuable for:
- regression detection
- architecture iteration guidance
- objective optimization goals

### 12.2 How we implemented it

`benchmarks/simple_benchmark.py`:
- opens TCP connection
- sends RESP `SET` workload
- measures per-request latency
- computes ops/sec, p50, p95
- outputs markdown report (`benchmarks/report.md`)

This gives immediate observability and a shared performance language for team discussions.

---

## 13) Developer Tooling and Operational Scripts

### 13.1 Concept

For team productivity, common operations should be scripted:
- build
- test
- run
- benchmark
- CI entrypoint

This minimizes setup variance and enables faster onboarding.

### 13.2 How we implemented it

Added scripts:
- `scripts/dev/build.sh`
- `scripts/dev/test.sh`
- `scripts/dev/run.sh`
- `scripts/dev/bench.sh`
- `scripts/ci/run_ci.sh`

Plus utility tooling:
- `tools/resp_inspector.py` for protocol payload debugging

Outcome:
- faster local workflows
- deterministic CI command path

---

## 14) Documentation and Spec Hygiene

### 14.1 Concept

A database project needs **operational docs**, not only code comments:
- setup steps
- command reference
- architecture narrative
- examples

Spec hygiene matters: command examples must be precise (length fields, reply shape), otherwise integration users fail unexpectedly.

### 14.2 How we implemented it

We added/updated:
- comprehensive setup and run guide in README
- protocol command reference (`docs/protocol/command-spec.md`)
- architecture overview
- examples README and Python client
- benchmark README

We also aligned README examples with command-spec semantics and corrected payload length mismatches (`makrounaDB` length = 10).

---

## 15) Repository Hygiene and Version Control Readiness

### 15.1 Concept

Clean repositories improve collaboration and reduce accidental noise in commits.

Key practices:
- remove placeholder artifacts once real files exist
- ignore generated/runtime files
- keep branch commits meaningful and scoped

### 15.2 How we implemented it

- removed all `.gitkeep` placeholder files
- deleted unused empty directories (`src/cluster`, `src/replication`, `tests/fixtures`)
- expanded `.gitignore` to exclude:
  - build outputs
  - AOF/runtime data
  - python caches/venv
  - editor metadata
  - benchmark-generated report output

Commit strategy:
- feature commit (core server and tests)
- docs/tooling commit
- repository hygiene commit

---

## 16) End-to-End Execution Walkthrough

### 16.1 Concept

A healthy system should have a deterministic local runbook from clone to live verification.

### 16.2 How we implemented it

Typical local flow:
1. Install prerequisites (`g++`, `cmake`, `ninja`, `python3`, `nc`)
2. Configure/build via CMake
3. Run CTest suite
4. Start server CLI
5. Validate via RESP with `nc` or example Python client
6. Optionally use NL shell mode for human-friendly interaction

This workflow is documented and reproducible.

---

## 17) What We Achieved (Technical Summary)

- Modular C++ Redis-inspired codebase with clear boundaries
- Real TCP server with stream-safe incremental parsing
- RESP protocol handling (requests + typed responses)
- Core command set with deterministic semantics
- TTL support with lazy + periodic expiration
- AOF persistence with startup replay
- Socket-level integration validation
- Natural-language command shell over real TCP/RESP
- Benchmark harness and operational script suite
- Cleaned repository and aligned technical documentation

---

## 18) Known Limitations and Engineering Tradeoffs

- Event loop currently `select`-based (good baseline, not highest scalability)
- Single-thread command path (predictable, simpler correctness model)
- AOF uses periodic flush (`everysec`) without advanced fsync policies
- Data model currently string-only for values
- No transactions, replication, or clustering yet (intentionally deferred)

These are expected for MVP and align with staged roadmap priorities.

---

## 19) Next Technical Milestones

1. **Protocol hardening**
   - broader malformed-frame coverage
   - additional edge-case tests

2. **Persistence evolution**
   - configurable durability modes
   - optional snapshot strategy

3. **Runtime evolution**
   - poll/epoll migration for larger connection sets
   - background worker channels for heavy maintenance tasks

4. **Feature expansion**
   - transaction primitives (`MULTI`/`EXEC`)
   - replication prototype

5. **Observability**
   - command-level metrics
   - structured logging and latency histogram export

---

## 20) Final Takeaway

makrounaDB is now a full-stack Redis-inspired system prototype with:
- protocol correctness mindset,
- modular C++ architecture,
- persistence + TTL lifecycles,
- strong test coverage layers,
- practical tooling and docs,
- and a developer-friendly CLI/NL interface.

It is both a strong educational implementation and a technically credible baseline for advanced distributed-database extensions.
