#!/bin/sh
# Compiles and runs the host test of the black bar detector.
set -e
cd "$(dirname "$0")/.."
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
cc -Wall -Wextra -Werror -Icommon/include -o "$OUT/test_bar_scan" scripts/test_bar_scan.c
"$OUT/test_bar_scan"
