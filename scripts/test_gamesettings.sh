#!/bin/sh
# Compiles and runs the host test of the per-game ini parser.
set -e
cd "$(dirname "$0")/.."
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
cc -Wall -Wextra -Werror -Iloader/source -o "$OUT/test_gamesettings" scripts/test_gamesettings.c loader/source/GameSettings.c
"$OUT/test_gamesettings"
