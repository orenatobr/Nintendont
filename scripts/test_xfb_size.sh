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
# The green block that flashed between two black screens when a game was chosen came from the loader turning the
# video back on for a few frames after the mode switch. The video must stay black until the game takes over.
! grep -n 'VIDEO_SetBlack(FALSE)' loader/source/main.c || fail "main.c turns the video on during the game launch"
# The loader leaves the VI off (power-on state) right before it jumps to the game, and passes the state to the kernel log.
grep -q 'vi\[1\] &= ~1' loader/source/main.c || fail "main.c does not switch the VI off before the jump"
grep -q 'Loader VI at launch' kernel/Patch.c || fail "the kernel does not log the loader VI state"
# The frame buffers of the loader are not freed by GRRLIB_Exit: the VI keeps scanning them while the loader waits for the kernel, and a freed buffer
# is reused (zeros, then data) and shows as a green band and noise. Only GRRLIB_ExitLight may free them (2 lines).
grep -q 'NIN_KEEP_XFB' loader/source/grrlib.c || fail "GRRLIB_Exit frees the frame buffers"
[ "$(grep -c 'free(MEM_K1_TO_K0(xfb' loader/source/grrlib.c)" = 2 ] || fail "more than the 2 frees of GRRLIB_ExitLight"
# fork.24 hung the loader: VIDEO_WaitVSync sleeps the thread, and the threads are closed before the jump
! grep -n 'SYS_SetArena1Hi' loader/source/main.c || fail "main.c parks the VI at the top of MEM1 again"
echo "ok"
