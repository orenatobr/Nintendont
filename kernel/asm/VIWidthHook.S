/*
VIWidthHook.S for Nintendont (Kernel), Teconmoon

Put at the start of the game's VIConfigure(rmode). r3 is the render mode object.
1. It leaves the pointer in shared memory (0xD3003F20) so that the kernel can log the video mode the game
   really uses, also when the game builds it at run time.
2. It sets the picture width (viWidth, +0xE) and the start (viXOrigin, +0xA) of that mode, which the patch of
   the static video tables does not reach. Only the video output reads these two fields, so the game's own
   drawing is not affected. The patcher turns the two stores into nops when no width is wanted.
r3 is checked before it is used: it has to point into the first 24 MB of cached memory and be a 640 pixel
wide mode. The first and last instruction and the two li values are written by the patcher.
*/
#include <asm.h>

VIWidthHook:
	nop		#0: original instruction, replaced by the patcher
	lis		r5,	0xD300	#1
	ori		r5,	r5,	0x3F20	#2
	stw		r3,	0(r5)		#3: the mode, for the kernel log
	lis		r4,	0x8000	#4
	cmplw	r3,	r4		#5
	blt		skip		#6
	lis		r4,	0x8180	#7
	cmplw	r3,	r4		#8
	bge		skip		#9
	lhz		r4,	4(r3)		#10: fbWidth
	cmpwi	r4,	640		#11
	bne		skip		#12
	li		r4,	0		#13: viWidth, replaced by the patcher
	sth		r4,	0xE(r3)	#14
	li		r4,	0		#15: viXOrigin, replaced by the patcher
	sth		r4,	0xA(r3)	#16
skip:
	nop		#17: jump back to VIConfigure + 4, replaced by the patcher
