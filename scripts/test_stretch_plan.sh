#!/bin/sh
# Compiles and runs the host test of the stretch plan.
set -e
cd "$(dirname "$0")/.."
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
cc -Wall -Wextra -Werror -Icommon/include -o "$OUT/test_stretch_plan" scripts/test_stretch_plan.c
"$OUT/test_stretch_plan"
