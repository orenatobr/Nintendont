#ifndef __STRETCH_PLAN_H__
#define __STRETCH_PLAN_H__

#include "BarScan.h"

/*
 * Plan of the automatic stretch (phases 2 and 3, not connected to the console yet).
 * From the black bars NinScanBars found, it says which part of the XFB to show and where on the
 * 720 pixel signal to put it:
 *  - Force Widescreen on: cut every bar and fill the whole screen. The output is NIN_FILL_VIDEO_SCALE + 600
 *    pixels wide, centered (688 pixels, origin 16).
 *  - Force Widescreen off: a 4:3 window. Bars above and below are cut, bars at the sides stay, and the
 *    output is the native 640 pixels at origin 40.
 * Phase 2 would apply srcX/srcW and the output width through the video interface; phase 3 would apply
 * srcY/srcH through the EFB copy, because the video interface cannot scale vertically.
 * A frame that is black everywhere, or bars that would leave less than half of the width or height, are
 * not trusted (a dark scene looks like bars) and give the whole picture.
 */
typedef struct NinStretch
{
	unsigned short srcX;	/* first XFB pixel to show */
	unsigned short srcW;	/* pixels to show */
	unsigned short srcY;	/* first XFB line to show */
	unsigned short srcH;	/* lines to show */
	unsigned short viWidth;		/* output width on the 720 pixel signal */
	unsigned short viXOrigin;	/* output start on the 720 pixel signal */
} NinStretch;

static inline NinStretch NinPlanStretch(const NinBars *bars, unsigned fbW, unsigned fbH, int forceWide)
{
	NinStretch s;
	unsigned l = bars->left, r = bars->right, t = bars->top, b = bars->bottom;
	s.srcX = 0; s.srcW = (unsigned short)fbW; s.srcY = 0; s.srcH = (unsigned short)fbH;
	if(bars->all_black || l + r >= fbW || t + b >= fbH
		|| (fbW - l - r) * 2 < fbW || (fbH - t - b) * 2 < fbH)
	{
		l = r = t = b = 0; /* nothing trusted */
	}
	if(forceWide)
	{
		s.srcX = (unsigned short)l;
		s.srcW = (unsigned short)(fbW - l - r);
		s.viWidth = (NIN_FILL_VIDEO_SCALE + 600);
	}
	else
		s.viWidth = 640;
	s.srcY = (unsigned short)t;
	s.srcH = (unsigned short)(fbH - t - b);
	s.viXOrigin = (unsigned short)((720 - s.viWidth) / 2);
	return s;
}

#endif
