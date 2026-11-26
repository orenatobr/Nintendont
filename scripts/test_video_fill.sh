#!/bin/sh
# Compiles and runs the host test of the Force Widescreen video fill helper.
set -e
cd "$(dirname "$0")/.."
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
cc -Wall -Wextra -Werror -Icommon/include -o "$OUT/test_video_fill" scripts/test_video_fill.c
"$OUT/test_video_fill"
