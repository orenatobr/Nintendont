#!/bin/sh
# Compiles and runs the host test of the letterbox table matcher.
set -e
cd "$(dirname "$0")/.."
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
cc -Wall -Wextra -Werror -Icommon/include -o "$OUT/test_letterbox_tables" scripts/test_letterbox_tables.c
"$OUT/test_letterbox_tables"
