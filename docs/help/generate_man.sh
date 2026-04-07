#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
INPUT="${ROOT_DIR}/docs/help/makrounaDB.1.md"
OUTPUT="${ROOT_DIR}/docs/help/makrounaDB.1"

if ! command -v pandoc >/dev/null 2>&1; then
  echo "error: pandoc is required to generate the man page" >&2
  exit 1
fi

pandoc --standalone --to man "${INPUT}" --output "${OUTPUT}"
echo "generated: ${OUTPUT}"
