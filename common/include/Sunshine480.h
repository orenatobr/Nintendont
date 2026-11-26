#ifndef __SUNSHINE_480_H__
#define __SUNSHINE_480_H__

/*
 * Super Mario Sunshine (NTSC-U) runs a 448 line video mode built at run time (XFB Scan log of the console:
 * VTR active lines 0x1C0, 32 empty lines of the 480 line signal). The video interface cannot scale
 * vertically, so the game has to be made to use 480 lines. The 448 comes from small functions of the game:
 *
 *   width and height getters:   li r3,640; blr; li r3,448; blr      (0x2A8BA0 and the like)
 *   NTSC branch of a height:    li r31,448; bl ...; cmpwi r3,2      (0x2A8BE8, 0x2A8C54)
 *                               li r31,448; b +8; li r31,0x212      (0x2A8C18, 0x2A8C84; 0x212 is PAL)
 *
 * NinPatch448Lines looks for exactly these shapes in an array of instruction words and changes the 448
 * (0x1C0) to 480 (0x1E0). The other uses of 448 in the game (allocation sizes, call arguments) do not match
 * and are not touched. Returns how many words were changed. The words are the PPC instruction values, so the
 * array has to be read in the PPC's (big endian) order, which the ARM kernel does.
 */
#define NIN_PPC_LI_R3_448	(0x386001C0)
#define NIN_PPC_LI_R3_640	(0x38600280)
#define NIN_PPC_LI_R31_448	(0x3BE001C0)
#define NIN_PPC_BLR		(0x4E800020)
#define NIN_PPC_CMPWI_R3_2	(0x2C030002)
#define NIN_PPC_LI_R31_530	(0x3BE00212)

static inline unsigned NinPatch448Lines(volatile unsigned *w, unsigned count)
{
	unsigned i, changed = 0;
	for(i = 2; i < count; i++)
	{
		unsigned x = w[i];
		if(x == NIN_PPC_LI_R3_448 && i + 1 < count && w[i + 1] == NIN_PPC_BLR && w[i - 1] == NIN_PPC_BLR && w[i - 2] == NIN_PPC_LI_R3_640)
		{
			w[i] = NIN_PPC_LI_R3_448 + 0x20;
			changed++;
		}
		else if(x == NIN_PPC_LI_R31_448 && i + 2 < count && ((w[i + 1] >> 26) == 18) && w[i + 2] == NIN_PPC_CMPWI_R3_2)
		{
			w[i] = NIN_PPC_LI_R31_448 + 0x20;
			changed++;
		}
		else if(x == NIN_PPC_LI_R31_448 && i + 2 < count && w[i + 1] == 0x48000008 && w[i + 2] == NIN_PPC_LI_R31_530)
		{
			w[i] = NIN_PPC_LI_R31_448 + 0x20;
			changed++;
		}
	}
	return changed;
}

#endif
