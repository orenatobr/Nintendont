/*
 * Host test for NinPatch448Lines (common/include/Sunshine480.h), with words taken from a real ndebug.log
 * of Super Mario Sunshine (NTSC-U), lines "Scan448:".
 * Run: scripts/test_sunshine_480.sh
 */
#include <assert.h>
#include <stdio.h>
#include "Sunshine480.h"

int main(void)
{
	/* the width and height getters of 0x2A8BA0: li r3,640; blr; li r3,448; blr; ... */
	unsigned getters[] = {0x38600280, 0x4E800020, 0x386001C0, 0x4E800020, 0x38600280, 0x4E800020, 0x386001C0, 0x4E800020};
	assert(NinPatch448Lines(getters, 8) == 2);
	assert(getters[2] == 0x386001E0 && getters[6] == 0x386001E0);
	assert(getters[0] == 0x38600280 && getters[4] == 0x38600280);
	/* the NTSC branch of the height functions: li r31,448; bl; cmpwi r3,2 */
	unsigned ntscA[] = {0x9421FFF0, 0x93E1000C, 0x3BE001C0, 0x480A7AB1, 0x2C030002, 0x41820024};
	assert(NinPatch448Lines(ntscA, 6) == 1 && ntscA[2] == 0x3BE001E0);
	/* li r31,448; b +8; li r31,0x212 (the PAL value stays) */
	unsigned ntscB[] = {0x41820008, 0x48000010, 0x3BE001C0, 0x48000008, 0x3BE00212, 0x80010014};
	assert(NinPatch448Lines(ntscB, 6) == 1 && ntscB[2] == 0x3BE001E0 && ntscB[4] == 0x3BE00212);
	/* other uses of 448 in the same game are not touched: allocation sizes and call arguments */
	unsigned other1[] = {0x4E800020, 0x7C0802A6, 0x386001C0, 0x90010004, 0x9421FFE8, 0x93E10014};
	unsigned other2[] = {0x2C030000, 0x40820028, 0x386001C0, 0x4810D311, 0x7C7E1B79, 0x41820010};
	unsigned other3[] = {0x806DA020, 0x389F02F0, 0x38A001C0, 0x48214649, 0x38000001, 0x981E0000};
	assert(NinPatch448Lines(other1, 6) == 0 && other1[2] == 0x386001C0);
	assert(NinPatch448Lines(other2, 6) == 0 && other2[2] == 0x386001C0);
	assert(NinPatch448Lines(other3, 6) == 0 && other3[2] == 0x38A001C0);
	/* a short buffer does not read outside */
	unsigned tiny[] = {0x386001C0, 0x4E800020};
	assert(NinPatch448Lines(tiny, 2) == 0);
	puts("test_sunshine_480: OK");
	return 0;
}
