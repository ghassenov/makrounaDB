# makrounaDB Project Plan (Redis from Scratch in C++)

This document defines:
1. the repository structure,
2. a clear division of work between 3 teammates,
3. a milestone-based execution plan inspired by *Build Your Own Redis with C/C++*.

---

## 1) Project Goal and Scope

**Goal:** Build a Redis-like in-memory key-value database in modern C++ (C++17/20), incrementally, with clear ownership and integration points.

**Primary MVP capabilities:**
- TCP server with multiple client support
- RESP parsing/serialization
- Core commands: `PING`, `ECHO`, `SET`, `GET`, `DEL`, `EXISTS`, `INCR`
- Expiration (`EXPIRE`, `TTL`) with lazy + periodic cleanup
- Basic persistence (AOF or snapshot, choose one first)
- Unit + integration tests for core flows

**Post-MVP capabilities (stretch):**
- Transactions (`MULTI/EXEC`)
- Replication basics
- Optional clustering research
- Benchmarking and optimization passes

---

## 2) Repository Structure

The following structure was created with empty directories tracked via `.gitkeep`:

- `docs/` - high-level documentation
  - `docs/architecture/` - architecture decisions and diagrams
  - `docs/protocol/` - RESP notes and command behavior specs
  - `docs/meeting-notes/` - weekly meeting outcomes and action items
- `include/` - public headers by module
  - `include/common/`
  - `include/network/`
  - `include/protocol/`
  - `include/storage/`
  - `include/command/`
- `src/` - implementation files by module
  - `src/app/` - server entrypoint/bootstrap
  - `src/common/` - shared utilities, error handling, logging
  - `src/network/` - socket/event loop/client handling
  - `src/protocol/` - RESP encoder/decoder
  - `src/storage/` - in-memory data model, TTL metadata
  - `src/command/` - command dispatch + handlers
  - `src/persistence/` - AOF/RDB-like persistence logic
  - `src/replication/` - replication prototype and sync logic
  - `src/cluster/` - optional cluster experiments
- `tests/` - all tests
  - `tests/unit/` - module-level tests
  - `tests/integration/` - end-to-end command and server tests
  - `tests/fixtures/` - test data and scripted scenarios
- `benchmarks/` - throughput/latency test scripts and reports
- `scripts/` - automation scripts
  - `scripts/dev/` - local dev helpers (`run`, `format`, `lint`)
  - `scripts/ci/` - CI scripts
- `config/` - runtime configuration templates
- `tools/` - helper tooling (load generator, protocol inspector)
- `examples/` - client examples and usage demos

---

## 3) Team Division (3-Person Ownership)

## Member A — Networking + Server Runtime Owner

**Owns:**
- `src/app/`, `src/network/`, `include/network/`
- Server lifecycle, sockets, client session management, event loop

**Responsibilities:**
- Build TCP server foundation and connection handling
- Integrate parser/dispatcher pipeline (`network -> protocol -> command`)
- Implement graceful startup/shutdown, basic config loading
- Add integration tests for concurrent clients

**Primary risks:** deadlocks, resource leaks, partial read/write handling

---

## Member B — Protocol + Command Execution Owner

**Owns:**
- `src/protocol/`, `src/command/`, `include/protocol/`, `include/command/`

**Responsibilities:**
- Implement RESP parser and serializer
- Build command registry/dispatcher
- Implement command handlers for MVP command set
- Define command behavior docs in `docs/protocol/`

**Primary risks:** protocol edge cases, malformed input handling, command consistency

---

## Member C — Storage + Persistence + QA Owner

**Owns:**
- `src/storage/`, `src/persistence/`, `include/storage/`
- `tests/`, `benchmarks/`, and test quality gates

**Responsibilities:**
- Implement in-memory keyspace model and TTL logic
- Implement first persistence strategy (recommended first: AOF)
- Build unit tests for storage and persistence paths
- Create baseline benchmark scenarios and report format

**Primary risks:** data consistency, persistence recovery correctness, TTL race conditions

---

## Shared/Rotating Areas

- `src/common/`, `include/common/`, `docs/architecture/`, `scripts/`, `tools/`
- Rotation each sprint so everyone contributes outside primary ownership

---

## 4) Milestone Plan (Suggested 8 Weeks)

## Phase 0 (Week 1) — Foundation

**Team outcomes:**
- Agree on coding standards, branch strategy, and CI checks
- Choose build system (`CMake` recommended)
- Create base app skeleton and module interfaces

**Ownership split:**
- A: socket server skeleton
- B: RESP framing prototype
- C: key-value store interface + test scaffold

---

## Phase 1 (Weeks 2-3) — MVP Core

**Team outcomes:**
- End-to-end request path works for basic commands
- Commands: `PING`, `ECHO`, `SET`, `GET`, `DEL`, `EXISTS`, `INCR`
- Unit tests for parser, command handlers, storage

**Ownership split:**
- A: stable client read/write loop + connection cleanup
- B: parser/serializer + dispatcher + command handlers
- C: storage map + integer ops + test coverage expansion

---

## Phase 2 (Weeks 4-5) — TTL + Reliability

**Team outcomes:**
- `EXPIRE`, `TTL`, lazy expiration, periodic cleanup task
- Better error handling, protocol validation, memory checks

**Ownership split:**
- A: scheduler/timer integration in server loop
- B: TTL command semantics + protocol error responses
- C: expiration index, cleanup strategy, edge-case tests

---

## Phase 3 (Weeks 6-7) — Persistence + Recovery

**Team outcomes:**
- AOF write path + replay on startup (or snapshot if chosen)
- Crash/restart recovery tested with integration scenarios

**Ownership split:**
- A: lifecycle hooks (flush on shutdown, startup replay trigger)
- B: persistence-related command hooks integration
- C: AOF format/write/replay + persistence test suite

---

## Phase 4 (Week 8) — Hardening + Demo

**Team outcomes:**
- Performance baseline (ops/sec, p50/p95 latency)
- Stability fixes from test/benchmark findings
- Final demo script and architecture write-up

**Ownership split:**
- A: load behavior and connection stability improvements
- B: protocol compliance polish and command edge cases
- C: benchmark report + regression checks

---

## 5) Collaboration Workflow

## Branching
- `main`: always stable
- Feature branches: `feature/<module>-<short-desc>`
- Bugfix branches: `fix/<module>-<short-desc>`

## Pull Requests
- Minimum 1 reviewer
- PR should include:
  - scope summary,
  - tests added/updated,
  - known limitations.

## Review Rules
- Owner of touched module must review
- No direct push to `main`
- Merge only when CI passes

## Weekly Cadence
- 2 short syncs/week (30-45 min)
- 1 planning/review checkpoint/week
- Meeting notes go to `docs/meeting-notes/`

---

## 6) Definition of Done (Per Feature)

A feature is done only if:
- implementation compiles,
- unit tests pass,
- integration scenario exists for behavior,
- protocol behavior is documented when relevant,
- code reviewed and merged via PR.

---

## 7) Risk Management

**Risk:** modules drift apart and fail integration  
**Mitigation:** weekly integration branch smoke test

**Risk:** too many features before MVP  
**Mitigation:** strict MVP freeze until Phase 3 complete

**Risk:** performance tuning too early  
**Mitigation:** first target correctness, benchmark only after MVP