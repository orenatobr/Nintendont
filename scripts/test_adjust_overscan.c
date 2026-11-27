/*
 * Host test for NinFindOverscanFuncs (common/include/AdjustOverscan.h). The words are built here to have the
 * shape of the SDK function GXAdjustForOverscan(rmin, rmout, hor, ver), which fills a video mode from another
 * one: a short function with a divwu (verf = ver2 * efbHeight / xfbHeight) that stores the fields at offsets
 * 4, 6, 8, 0xA, 0xC, 0xE and 0x10 of the mode with sth.
 * Run: scripts/test_adjust_overscan.sh
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "AdjustOverscan.h"

#define BLR	(0x4E800020)
#define NOP	(0x60000000)
#define STH(rs, ra, off)	(0xB0000000u | ((rs) << 21) | ((ra) << 16) | (off))
#define DIVWU(rd, ra, rb)	(0x7C000396u | ((rd) << 21) | ((ra) << 16) | ((rb) << 11))

static unsigned build(unsigned *w, const unsigned *offs, unsigned n, int withDiv)
{
	unsigned i = 0, k;
	w[i++] = BLR;			/* the end of the function before */
	w[i++] = 0x9421FFF0;		/* stwu r1,-16(r1) */
	w[i++] = NOP;
	if(withDiv)
		w[i++] = DIVWU(8, 9, 10);
	for(k = 0; k < n; k++)
	{
		w[i++] = NOP;
		w[i++] = STH(5, 4, offs[k]);
	}
	w[i++] = 0x38210010;		/* addi r1,r1,16 */
	w[i++] = BLR;
	return i;
}

int main(void)
{
	unsigned w[64], starts[4], found;
	const unsigned all[] = {4, 6, 8, 0xA, 0xC, 0xE, 0x10};
	const unsigned five[] = {4, 6, 8, 0xA, 0xC};
	unsigned n;
	/* the real shape: one candidate, starting after the blr of the function before */
	n = build(w, all, 7, 1);
	found = NinFindOverscanFuncs(w, n, starts, 4, 7);
	assert(found == 1 && starts[0] == 1);
	/* no divwu: not it */
	n = build(w, all, 7, 0);
	assert(NinFindOverscanFuncs(w, n, starts, 4, 7) == 0);
	/* only five of the seven offsets: not it with 7 required, but listed as a near miss with 5 */
	n = build(w, five, 5, 1);
	assert(NinFindOverscanFuncs(w, n, starts, 4, 7) == 0);
	assert(NinFindOverscanFuncs(w, n, starts, 4, 5) == 1);
	/* a function that is too long is not it */
	{
		unsigned big[400], i = 0, k;
		big[i++] = BLR;
		big[i++] = DIVWU(8, 9, 10);
		for(k = 0; k < 7; k++) big[i++] = STH(5, 4, all[k]);
		for(k = 0; k < 300; k++) big[i++] = NOP;
		big[i++] = BLR;
		assert(NinFindOverscanFuncs(big, i, starts, 4, 7) == 0);
	}
	/* two candidates are both reported, the caller decides */
	{
		unsigned two[128], a = build(two, all, 7, 1), b = build(two + a - 1, all, 7, 1);
		assert(NinFindOverscanFuncs(two, a + b - 1, starts, 4, 7) == 2);
	}
	puts("test_adjust_overscan: OK");
	return 0;
}
