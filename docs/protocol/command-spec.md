# makrounaDB Command Spec

This document is the practical reference for interacting with the server.

## Protocol

Preferred format is RESP array-of-bulk-strings:

`*<argc>\r\n$<len>\r\n<arg>\r\n...`

Reply types:
- Simple string: `+...\r\n`
- Error: `-ERR ...\r\n`
- Integer: `:<n>\r\n`
- Bulk string: `$<len>\r\n...\r\n`
- Null bulk string: `$-1\r\n`

## Supported commands

### `PING [message]`
- No args: returns `+PONG`
- One arg: returns arg as bulk string

### `ECHO message`
- Returns `message` as bulk string

### `SET key value`
- Stores string value
- Returns `+OK`

### `GET key`
- Returns bulk string if exists
- Returns `$-1` if missing/expired

### `DEL key [key ...]`
- Deletes one or more keys
- Returns number of removed keys as integer

### `EXISTS key [key ...]`
- Returns count of existing keys as integer

### `INCR key`
- If key missing: initialize to `0` then increment
- Returns incremented integer value
- Error if value is not an integer

### `EXPIRE key seconds`
- `1` if expiration was set (or key removed when `seconds <= 0`)
- `0` if key does not exist

### `TTL key`
- `-2` if key does not exist (or already expired)
- `-1` if key exists with no expiration
- `>= 0` remaining seconds otherwise

## Copy-paste test commands (`nc`)

```bash
printf '*1\r\n$4\r\nPING\r\n' | nc -N 127.0.0.1 6379
printf '*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$10\r\nmakrounaDB\r\n' | nc -N 127.0.0.1 6379
printf '*2\r\n$3\r\nGET\r\n$4\r\nname\r\n' | nc -N 127.0.0.1 6379
printf '*2\r\n$4\r\nINCR\r\n$7\r\ncounter\r\n' | nc -N 127.0.0.1 6379
printf '*3\r\n$6\r\nEXPIRE\r\n$4\r\nname\r\n$2\r\n10\r\n' | nc -N 127.0.0.1 6379
printf '*2\r\n$3\r\nTTL\r\n$4\r\nname\r\n' | nc -N 127.0.0.1 6379
```

## Notes

- Successful mutating commands are appended to AOF (`SET`, `DEL`, `INCR`, `EXPIRE`).
- AOF is replayed on startup before accepting new writes.
