% makrounaDB(1) makrounaDB 0.1.0
% makrounaDB maintainers
% April 2026

# NAME

makrounaDB - Redis-inspired in-memory database server with RESP2 support

# SYNOPSIS

**makrounaDB** [**--port** *PORT*] [**--aof** *PATH*] [**--no-banner**]

**makrounaDB** **--nl-shell** [**--host** *IP*] [**--port** *PORT*]

**makrounaDB** [**-h**|**--help**]

# DESCRIPTION

`makrounaDB` is a lightweight, Redis-inspired database server written in modern C++.
It supports RESP2-style requests, string key/value operations, key expiration (TTL),
and append-only file persistence (AOF).

# OPTIONS

**--port** *PORT*
: TCP port to listen on (default: `6379`).

**--host** *IP*
: Host used only with `--nl-shell` mode (default: `127.0.0.1`).

**--aof** *PATH*
: AOF file path (default: `data/appendonly.aof`).

**--nl-shell**
: Start natural-language shell client mode.

**--no-banner**
: Disable startup ASCII banner.

**-h**, **--help**
: Show help message and exit.

# COMMANDS

Core commands:

- `PING [message]`
- `ECHO message`
- `SET key value`
- `GET key`
- `DEL key [key ...]`
- `EXISTS key [key ...]`
- `INCR key`
- `EXPIRE key seconds`
- `TTL key`

Pasta-themed aliases:

- `RIGATONI key` (DECR)
- `LINGUINE key amount` (INCRBY)
- `VERMICELLI key amount` (DECRBY)
- `SPAGHETTI key` (STRLEN)
- `PENNE key value` (SETNX)
- `ALDENTE key` (PERSIST)
- `FARFALLE key` (GETDEL)
- `LASAGNA key newkey` (RENAME)

# EXAMPLES

Run server on default port:

`makrounaDB`

Run on a custom port and AOF path:

`makrounaDB --port 6380 --aof /data/appendonly.aof`

Start natural language shell:

`makrounaDB --nl-shell --host 127.0.0.1 --port 6379`

# FILES

`data/appendonly.aof`
: Default append-only persistence file.

# EXIT STATUS

**0**
: Successful execution.

**1**
: Invalid CLI arguments, networking errors, or runtime failures.

# SEE ALSO

`nc(1)`, `redis-cli(1)`
