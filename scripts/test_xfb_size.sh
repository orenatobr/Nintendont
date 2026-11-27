#!/bin/sh
# Regression check for the green stripe at the bottom of the screen (upstream issue 1301): the loader frame
# buffers must be sized and cleared for the tallest video mode, not for the menu mode only. Zeroed YUV memory
# is green, so any line the VI scans beyond the cleared area shows as a green stripe.
set -e
cd "$(dirname "$0")/.."
fail() { echo "FAIL: $1" >&2; exit 1; }
! grep -n 'SYS_AllocateFramebuffer(rmode)' loader/source/*.c || fail "frame buffer sized by the menu mode"
! grep -n 'VIDEO_ClearFrameBuffer(rmode' loader/source/*.c || fail "frame buffer cleared by the menu mode"
grep -q 'NIN_XFB_MODE' loader/source/grrlib.c || fail "grrlib.c does not use NIN_XFB_MODE"
grep -q 'NIN_XFB_MODE' loader/source/main.c || fail "main.c does not use NIN_XFB_MODE"
grep -q 'define NIN_XFB_MODE' loader/include/global.h || fail "NIN_XFB_MODE is not defined"
echo "ok"
