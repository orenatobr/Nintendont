/*
VIAdjustHook.S for Nintendont (Kernel), Teconmoon

Put at the start of the game's GXAdjustForOverscan(rmin, rmout, hor, ver). `ver` (r6) is the number of lines
taken off the top and the bottom of the video mode (16 turns a 480 line mode into a 448 line mode with a letterbox).
It is set to 0, so the mode keeps its 480 lines and everything the game builds from it (the EFB copy, the XFB
size, the video interface) is consistent. The first instruction is the original one and the last is the jump back,
both written by the patcher.
*/
#include <asm.h>

VIAdjustHook:
	nop		#original instruction, replaced by the patcher
	li		r6,	0
	nop		#jump back to the function + 4, replaced by the patcher
