/*
VIWidthHook.S for Nintendont (Kernel), Teconmoon

Put at the start of the game's VIConfigure(rmode). r3 is the render mode object. It sets the picture
width (viWidth, +0xE) and the start (viXOrigin, +0xA) for modes that the game builds at run time, which the
patch of the static video tables does not reach. Only the video output reads these two fields, so the game's
own drawing is not affected. The two li values are written by the patcher (width and origin), and the first
and last instruction too (the original first instruction of VIConfigure, and the jump back).
r3 is checked before it is used: it has to point into the first 24 MB of cached memory and be a 640 pixel
wide mode.
*/
#include <asm.h>

VIWidthHook:
	nop		#original instruction, replaced by the patcher
	lis		r4,	0x8000
	cmplw	r3,	r4
	blt		skip
	lis		r4,	0x8180
	cmplw	r3,	r4
	bge		skip
	lhz		r4,	4(r3)		#fbWidth
	cmpwi	r4,	640
	bne		skip
	li		r4,	0		#viWidth, replaced by the patcher
	sth		r4,	0xE(r3)
	li		r4,	0		#viXOrigin, replaced by the patcher
	sth		r4,	0xA(r3)
skip:
	nop		#jump back to VIConfigure + 4, replaced by the patcher
