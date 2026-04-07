#!/usr/bin/env bash
set -euo pipefail

PORT="${1:-6379}"
AOF_PATH="${2:-data/appendonly.aof}"

./build/makrounaDB --port "$PORT" --aof "$AOF_PATH"
