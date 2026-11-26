#!/bin/sh
# Compiles and runs the host test of the Sunshine 448 to 480 signature matcher.
set -e
cd "$(dirname "$0")/.."
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
cc -Wall -Wextra -Werror -Icommon/include -o "$OUT/test_sunshine_480" scripts/test_sunshine_480.c
"$OUT/test_sunshine_480"
