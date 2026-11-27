#!/bin/sh
# Compiles and runs the host test of the GXAdjustForOverscan finder.
set -e
cd "$(dirname "$0")/.."
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
cc -Wall -Wextra -Werror -Icommon/include -o "$OUT/test_adjust_overscan" scripts/test_adjust_overscan.c
"$OUT/test_adjust_overscan"
