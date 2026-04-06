# makrounaDB Architecture Overview

## Runtime model

- Single process TCP server.
- Main loop uses `select` for readiness-based socket handling.
- Client input is buffered per connection and parsed incrementally.
- Request path: `network -> protocol parser -> command dispatcher -> storage/persistence -> RESP reply`.

## Storage model

- In-memory keyspace based on `std::unordered_map<std::string, std::string>`.
- TTL metadata stored in a dedicated expiration map.
- Lazy expiration on key access (`GET`, `EXISTS`, `TTL`).
- Periodic bounded cleanup executed from the event loop.

## Persistence model (AOF-first)

- Write commands are appended in RESP-array format to AOF.
- Startup replays AOF through the same command dispatcher.
- Periodic flush policy is `everysec`.
- Invalid/incomplete AOF replay currently fails gracefully with a warning.

## Module map

- `src/app`: CLI entrypoint and lifecycle.
- `src/network`: TCP server/event loop.
- `src/protocol`: RESP request parser and reply encoder.
- `src/command`: command routing and semantics.
- `src/storage`: keyspace + TTL behavior.
- `src/persistence`: append-only file persistence.

## Next milestones

1. Add parser hardening tests for malformed frames and large payload boundaries.
2. Add strict command error parity matrix against Redis RESP2 behavior.
3. Add worker-pool-backed background maintenance for future heavy tasks.
4. Add optional snapshot persistence mode.
