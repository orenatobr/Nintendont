/*
 * Host test for the per-game ini parser (loader/source/GameSettings.c).
 * Run: scripts/test_gamesettings.sh
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "GameSettings.h"

static int parse(const char *text, signed char out[GS_COUNT])
{
	char buf[512];
	strncpy(buf, text, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = 0;
	return GameSettingsParse(buf, out);
}

int main(void)
{
	signed char s[GS_COUNT];
	/* XfbScan switches the experimental bar scan on or off for this game; missing means leave it */
	assert(parse("[Teconmoon]\nXfbScan = True\n", s) == 1 && s[GS_XFB_SCAN] == 1);
	assert(parse("XfbScan = False\n", s) == 1 && s[GS_XFB_SCAN] == 0);
	assert(parse("wideScreenHack = True\n", s) == 1 && s[GS_XFB_SCAN] == -1);
	/* the existing keys keep working next to it */
	assert(parse("wideScreenHack = True\nVideoScale = 88\nXfbScan = 1\n", s) == 3);
	assert(s[GS_FORCE_WIDE] == 1 && s[GS_VIDEO_SCALE] == 88 && s[GS_XFB_SCAN] == 1);
	puts("test_gamesettings: OK");
	return 0;
}
