/*
 * Host test for NinPatchLetterboxTables (common/include/LetterboxTables.h). The fixture is the shape of the
 * 448 line table of Tales of Symphonia seen in a real ndebug.log ("VI mode:" line: fb=640 efb=448 xfb=448
 * yorig=16 w=640 h=448).
 * Run: scripts/test_letterbox_tables.sh
 */
#include <assert.h>
#include <stdio.h>
#include "LetterboxTables.h"

int main(void)
{
	/* viTVmode, fbWidth|efbHeight, xfbHeight|viXOrigin, viYOrigin|viWidth, viHeight|?, xfbMode, ... */
	unsigned t448[] = {0x00000000, 0x028001C0, 0x01C00028, 0x00100280, 0x01C00000, 0x00000001, 0, 0, 0, 0};
	assert(NinPatchLetterboxTables(t448, 10) == 1);
	assert(t448[1] == 0x028001C0);			/* efbHeight stays 448: the game scales the copy by xfb / efb */
	assert(t448[2] == 0x01E00028);			/* xfbHeight 480 */
	assert(t448[3] == 0x00000280);			/* viYOrigin 0 */
	assert(t448[4] == 0x01E00000);			/* viHeight 480 */
	/* a table that already has 480 lines is left alone */
	unsigned t480[] = {0x00000000, 0x028001E0, 0x01E00028, 0x00000280, 0x01E00000, 0x00000001, 0, 0, 0, 0};
	assert(NinPatchLetterboxTables(t480, 10) == 0 && t480[2] == 0x01E00028);
	/* 448 lines that start at line 0, or whose xfb and VI height differ, are not letterbox tables */
	unsigned noOrigin[] = {0x00000000, 0x028001C0, 0x01C00028, 0x00000280, 0x01C00000, 0x00000001, 0, 0, 0, 0};
	unsigned mixed[] = {0x00000000, 0x028001C0, 0x01C00028, 0x00100280, 0x01E00000, 0x00000001, 0, 0, 0, 0};
	assert(NinPatchLetterboxTables(noOrigin, 10) == 0);
	assert(NinPatchLetterboxTables(mixed, 10) == 0);
	/* two tables in a row, and a short buffer is not read past its end */
	unsigned two[20];
	int i;
	for(i = 0; i < 10; i++) { two[i] = t448[i]; two[10 + i] = t448[i]; }
	two[2] = two[12] = 0x01C00028; two[3] = two[13] = 0x00100280; two[4] = two[14] = 0x01C00000;
	assert(NinPatchLetterboxTables(two, 20) == 2);
	unsigned tiny[] = {0x00000000, 0x028001C0, 0x01C00028};
	assert(NinPatchLetterboxTables(tiny, 3) == 0);
	puts("test_letterbox_tables: OK");
	return 0;
}
