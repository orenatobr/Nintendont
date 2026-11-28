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
# The VI is NOT switched off before the jump (fork.23 did: DCR 1 to 0, and the game turns it on again, a candidate for the short vertical glitch), only logged.
! grep -q 'vi\[1\] &= ~1' loader/source/main.c || fail "main.c switches the VI off before the jump again"
grep -q 'Loader VI at launch' kernel/Patch.c || fail "the kernel does not log the loader VI state"
# An autoboot launch keeps the VI black: GRRLIB_Render must not unblank when nin_keep_black is set (the unblank of GRRLIB_Exit is the green band).
grep -q 'if (!nin_keep_black)' loader/source/grrlib.c || fail "GRRLIB_Render unblanks the VI on autoboot"
grep -q 'nin_keep_black = argsboot' loader/source/main.c || fail "main.c does not set nin_keep_black"
# A Virtual Console launch (autoboot) must not change the VI width in the menu video setup; the VI registers are logged at 4 steps.
grep -q 'if(argsboot == false)' loader/source/main.c || fail "main.c has no autoboot guard"
tr -d '\r' < loader/source/main.c | grep -A1 'if(argsboot == false)$' | grep -q 'ReconfigVideo(rmode)' || fail "ReconfigVideo(rmode) is not guarded by autoboot"
[ "$(grep -c 'SnapVI(' loader/source/main.c)" = 5 ] || fail "SnapVI is not called at the 4 steps"
grep -q 'Loader VI step' kernel/Patch.c || fail "the kernel does not log the VI steps"
# HW_PPCSPEED is written again (the fork.30 log showed it changes nothing on a VC launch) and the loader pause of fork.30 is gone.
! grep -q 'sleep(8)' loader/source/main.c || fail "the bisect pause is still there"
! grep -q 'skipped on Wii VC' kernel/main.c || fail "the kernel still skips HW_PPCSPEED"
# A Virtual Console (autoboot) launch skips the GPU init (GX_Init) in the loader (fork.32 experiment); audio is initialised as always.
grep -q 'if (nin_keep_black)' loader/source/grrlib.c || fail "GRRLIB_Init does not skip GX in autoboot"; [ "$(grep -c 'nin_keep_black)' loader/source/grrlib.c)" -ge 3 ] || fail "GX skip missing in Init/Exit/Render"
# fork.33 diagnostic: the kernel samples the VI state with timestamps.
tr -d '\r' < kernel/main.c | grep -q 'ViSample();' || fail "kernel does not sample the VI state"
echo "ok"
