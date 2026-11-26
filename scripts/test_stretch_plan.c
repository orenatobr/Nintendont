/*
 * Host test for NinPlanStretch (common/include/StretchPlan.h).
 * Run: scripts/test_stretch_plan.sh
 */
#include <assert.h>
#include <stdio.h>
#include "CommonConfig.h"
#include "BarScan.h"
#include "StretchPlan.h"

static NinBars bars(unsigned l, unsigned r, unsigned t, unsigned b)
{
	NinBars x = {(unsigned short)l, (unsigned short)r, (unsigned short)t, (unsigned short)b, 0};
	return x;
}

int main(void)
{
	NinBars b;
	NinStretch s;
	/* no bars: the whole picture, in both modes */
	b = bars(0, 0, 0, 0);
	s = NinPlanStretch(&b, 640, 480, 1);
	assert(s.srcX == 0 && s.srcW == 640 && s.srcY == 0 && s.srcH == 480);
	s = NinPlanStretch(&b, 640, 480, 0);
	assert(s.srcX == 0 && s.srcW == 640 && s.srcY == 0 && s.srcH == 480);
	/* Force Widescreen on: every bar is cut and the output fills the screen (688 px, origin 16) */
	b = bars(80, 80, 0, 0);
	s = NinPlanStretch(&b, 640, 480, 1);
	assert(s.srcX == 80 && s.srcW == 480 && s.srcY == 0 && s.srcH == 480);
	assert(s.viWidth == 688 && s.viXOrigin == 16);
	b = bars(40, 24, 16, 16);
	s = NinPlanStretch(&b, 640, 480, 1);
	assert(s.srcX == 40 && s.srcW == 576 && s.srcY == 16 && s.srcH == 448);
	/* Force Widescreen off: a 4:3 window. Bars above and below are cut, side bars stay, output is 640 px at origin 40 */
	b = bars(80, 80, 32, 48);
	s = NinPlanStretch(&b, 640, 480, 0);
	assert(s.srcX == 0 && s.srcW == 640 && s.srcY == 32 && s.srcH == 400);
	assert(s.viWidth == 640 && s.viXOrigin == 40);
	/* a black frame says nothing about bars: leave the picture alone */
	b = bars(0, 0, 0, 0);
	b.all_black = 1;
	s = NinPlanStretch(&b, 640, 480, 1);
	assert(s.srcX == 0 && s.srcW == 640 && s.srcY == 0 && s.srcH == 480);
	/* a dark scene (bars eating more than half the picture) is not trusted */
	b = bars(400, 100, 0, 0);
	s = NinPlanStretch(&b, 640, 480, 1);
	assert(s.srcX == 0 && s.srcW == 640);
	b = bars(0, 0, 300, 100);
	s = NinPlanStretch(&b, 640, 480, 0);
	assert(s.srcY == 0 && s.srcH == 480);
	puts("test_stretch_plan: OK");
	return 0;
}
