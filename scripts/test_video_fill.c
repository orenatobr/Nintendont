/*
 * Host test for NinEffectiveVideoScale (common/include/CommonConfig.h).
 * Run: scripts/test_video_fill.sh
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "CommonConfig.h"

static signed char scale_for(signed char scale, unsigned int config)
{
	NIN_CFG cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.VideoScale = scale;
	cfg.Config = config;
	return NinEffectiveVideoScale(&cfg);
}

int main(void)
{
	/* 4:3 window: Force Widescreen off and no width chosen leaves the game's own table alone (0 = auto) */
	assert(scale_for(0, 0) == 0);
	/* Force Widescreen on and no width chosen fills the screen: 600 + 88 = 688 pixels */
	assert(scale_for(0, NIN_CFG_FORCE_WIDE) == NIN_FILL_VIDEO_SCALE);
	assert(NIN_FILL_VIDEO_SCALE + 600 == 688);
	/* a width the user chose always wins, with or without Force Widescreen */
	assert(scale_for(100, 0) == 100);
	assert(scale_for(100, NIN_CFG_FORCE_WIDE) == 100);
	assert(scale_for(40, NIN_CFG_FORCE_WIDE) == 40);
	assert(scale_for(120, 0) == 120);
	/* values outside 40..120 are not a chosen width */
	assert(scale_for(39, 0) == 0);
	assert(scale_for(121, 0) == 0);
	assert(scale_for(-5, NIN_CFG_FORCE_WIDE) == NIN_FILL_VIDEO_SCALE);
	assert(scale_for(39, NIN_CFG_FORCE_WIDE) == NIN_FILL_VIDEO_SCALE);
	/* other option bits do not matter */
	assert(scale_for(0, NIN_CFG_FORCE_PROG) == 0);
	/* vertical: a letterboxed table (448 lines starting at line 16) is stretched to all 480 lines */
	{
		unsigned short xfb = 448, yorig = 16, vh = 448;
		assert(NinFillVertical(&xfb, &yorig, &vh) == 1);
		assert(xfb == 480 && yorig == 0 && vh == 480);
	}
	/* a table that already uses all 480 lines is left alone */
	{
		unsigned short xfb = 480, yorig = 0, vh = 480;
		assert(NinFillVertical(&xfb, &yorig, &vh) == 0);
		assert(xfb == 480 && yorig == 0 && vh == 480);
	}
	/* PAL 528 lines, tall tables and tables whose xfb differs from the VI height are left alone */
	{
		unsigned short xfb = 528, yorig = 23, vh = 528;
		assert(NinFillVertical(&xfb, &yorig, &vh) == 0);
		xfb = 240; yorig = 8; vh = 480;
		assert(NinFillVertical(&xfb, &yorig, &vh) == 0);
		assert(xfb == 240 && yorig == 8 && vh == 480);
	}
	puts("test_video_fill: OK");
	return 0;
}
