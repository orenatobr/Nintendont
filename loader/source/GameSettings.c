#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "GameSettings.h"

static char *Trim(char *s)
{
	while (*s == ' ' || *s == '\t')
		s++;
	char *e = s + strlen(s);
	while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r'))
		*--e = 0;
	return s;
}

/* 1 for True, 1 and Yes; 0 for False, 0 and No; -1 for anything else */
static int Bool(const char *v)
{
	if (!strcasecmp(v, "true") || !strcmp(v, "1") || !strcasecmp(v, "yes"))
		return 1;
	if (!strcasecmp(v, "false") || !strcmp(v, "0") || !strcasecmp(v, "no"))
		return 0;
	return -1;
}

/* the number in s when it is between lo and hi, plus bias; -1 otherwise */
static int Num(const char *s, int lo, int hi, int bias)
{
	char *e;
	long n = strtol(s, &e, 10);
	if (e == s || *e || n < lo || n > hi)
		return -1;
	return (int)n + bias;
}

int GameSettingsParse(char *text, signed char out[GS_COUNT])
{
	int i, n = 0;
	for (i = 0; i < GS_COUNT; i++)
		out[i] = -1;
	while (text && *text)
	{
		char *next = strchr(text, '\n');
		if (next)
			*next++ = 0;
		char *line = Trim(text);
		text = next;
		char *eq = strchr(line, '=');
		if (line[0] == '#' || line[0] == ';' || line[0] == '[' || !eq)
			continue;
		*eq = 0;
		char *key = Trim(line);
		char *val = Trim(eq + 1);
		int which = -1, v = -1;
		if (!strcasecmp(key, "AspectRatio"))
		{
			which = GS_WIIU_WIDE;
			int a = atoi(val);
			v = a == 1 ? 1 : a == 2 ? 0 : -1;
		}
		else if (!strcasecmp(key, "wideScreenHack"))
			which = GS_FORCE_WIDE, v = Bool(val);
		else if (!strcasecmp(key, "ProgressiveScan"))
			which = GS_FORCE_PROG, v = Bool(val);
		else if (!strcasecmp(key, "SkipIPL"))
			which = GS_SKIP_IPL, v = Bool(val);
		else if (!strcasecmp(key, "PatchPAL50"))
			which = GS_PATCH_PAL50, v = Bool(val);
		else if (!strcasecmp(key, "Deflicker"))
			which = GS_DEFLICKER, v = Bool(val);
		else if (!strcasecmp(key, "VideoScale"))
			which = GS_VIDEO_SCALE, v = (!strcmp(val, "0") || !strcasecmp(val, "auto")) ? 0 : Num(val, 40, 120, 0);
		else if (!strcasecmp(key, "VideoOffset"))
			which = GS_VIDEO_OFFSET, v = Num(val, -20, 20, 64);
		else if (!strcasecmp(key, "XfbScan"))
			which = GS_XFB_SCAN, v = Bool(val);
		else if (!strcasecmp(key, "VideoMode"))
		{
			which = GS_VIDEO_MODE;
			v = !strcasecmp(val, "auto") ? 0 : !strcasecmp(val, "pal50") ? 1 : !strcasecmp(val, "pal60") ? 2 :
			    !strcasecmp(val, "ntsc") ? 3 : !strcasecmp(val, "mpal") ? 4 : -1;
		}
		if (which >= 0 && v >= 0)
		{
			if (out[which] < 0)
				n++;
			out[which] = (signed char)v;
		}
	}
	return n;
}
