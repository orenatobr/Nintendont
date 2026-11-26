#ifndef __ADJUST_OVERSCAN_H__
#define __ADJUST_OVERSCAN_H__

/*
 * The SDK function GXAdjustForOverscan(rmin, rmout, hor, ver) fills a video mode from another one and takes
 * `ver` lines off the top and the bottom: efbHeight - (2 * ver * efbHeight / xfbHeight), xfbHeight and viHeight
 * - 2 * ver, viYOrigin + ver. With ver = 16 a 480 line mode becomes the 448 line mode with a letterbox that
 * Tales of Symphonia shows in its log (efb 448, xfb 448, yorig 16, height 448, from tables of 480 lines).
 *
 * NinFindOverscanFuncs finds that function by its shape in an array of instruction words: a function (the words
 * between two blr) of 8 to 128 words, with a divwu, that stores with sth the fields at the offsets 4, 6, 8, 0xA,
 * 0xC, 0xE and 0x10 of a mode. A candidate needs at least minOffsets of the seven. Fills starts[] with the index of
 * the first word of each candidate (up to max) and returns how many were found (not capped). The words are the PPC
 * instruction values, which the ARM kernel reads in the PPC's (big endian) order.
 */
static inline unsigned NinFindOverscanFuncs(const volatile unsigned *w, unsigned count, unsigned *starts, unsigned max, unsigned minOffsets)
{
	unsigned i, start = 0, found = 0;
	for(i = 0; i < count; i++)
	{
		if(w[i] != 0x4E800020)	/* blr */
			continue;
		if(i > start + 8 && i - start <= 128)
		{
			unsigned k, mask = 0, hasDiv = 0, n = 0;
			for(k = start; k < i; k++)
			{
				unsigned x = w[k];
				if((x & 0xFC0007FE) == 0x7C000396)	/* divwu */
					hasDiv = 1;
				else if((x >> 26) == 44)		/* sth */
				{
					unsigned off = x & 0xFFFF;
					if(off == 4) mask |= 1; else if(off == 6) mask |= 2; else if(off == 8) mask |= 4;
					else if(off == 0xA) mask |= 8; else if(off == 0xC) mask |= 16; else if(off == 0xE) mask |= 32;
					else if(off == 0x10) mask |= 64;
				}
			}
			for(k = 0; k < 7; k++)
				n += (mask >> k) & 1;
			if(hasDiv && n >= minOffsets)
			{
				if(found < max)
					starts[found] = start;
				found++;
			}
		}
		start = i + 1;
	}
	return found;
}

#endif
