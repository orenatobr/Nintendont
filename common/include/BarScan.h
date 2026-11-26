#ifndef __BAR_SCAN_H__
#define __BAR_SCAN_H__

/*
 * Black bar detector for a YUY2 frame (the GameCube/Wii external framebuffer, XFB).
 * The Y of pixel x of a row is the byte at 2 * x, so only every second byte is read.
 * A pixel is black when Y <= NIN_BAR_BLACK_Y. A row or column belongs to a bar when at
 * least NIN_BAR_PERCENT percent of its pixels are black, so a little noise does not break a bar.
 * Bars are counted from each edge inwards, in pixels (left, right) and lines (top, bottom).
 * A frame that is black everywhere is reported as all_black, with every bar 0, because it
 * says nothing about bars.
 * The scan starts at the edges and stops at the first row or column that is not black, so it
 * needs no work buffers and reads little when the bars are thin. It is meant to run in the
 * PPC hook of the game, where the small stack and the 12 KB of code room do not allow tables.
 */
#define NIN_BAR_BLACK_Y		(0x18)
#define NIN_BAR_PERCENT		(98)

typedef struct NinBars
{
	unsigned short left;
	unsigned short right;
	unsigned short top;
	unsigned short bottom;
	unsigned char all_black;
} NinBars;

/* 1 when at least NIN_BAR_PERCENT percent of the width pixels of line y are black */
static inline int NinRowIsBlack(const unsigned char *xfb, unsigned width, unsigned stride, unsigned y)
{
	const unsigned char *row = xfb + y * stride;
	unsigned x, count = 0;
	for(x = 0; x < width; x++)
		if(row[2 * x] <= NIN_BAR_BLACK_Y)
			count++;
	return count * 100 >= width * NIN_BAR_PERCENT;
}

/* 1 when at least NIN_BAR_PERCENT percent of the height pixels of column x are black */
static inline int NinColIsBlack(const unsigned char *xfb, unsigned height, unsigned stride, unsigned x)
{
	unsigned y, count = 0;
	for(y = 0; y < height; y++)
		if(xfb[y * stride + 2 * x] <= NIN_BAR_BLACK_Y)
			count++;
	return count * 100 >= height * NIN_BAR_PERCENT;
}

static inline NinBars NinScanBars(const unsigned char *xfb, unsigned width, unsigned height, unsigned stride)
{
	NinBars bars = {0, 0, 0, 0, 0};
	unsigned n;
	if(width == 0 || height == 0)
		return bars;
	for(n = 0; n < height && NinRowIsBlack(xfb, width, stride, n); n++)
		;
	if(n == height)
	{
		bars.all_black = 1;
		return bars;
	}
	bars.top = (unsigned short)n;
	for(n = 0; n < height && NinRowIsBlack(xfb, width, stride, height - 1 - n); n++)
		;
	bars.bottom = (unsigned short)n;
	for(n = 0; n < width && NinColIsBlack(xfb, height, stride, n); n++)
		;
	bars.left = (unsigned short)n;
	for(n = 0; n < width && NinColIsBlack(xfb, height, stride, width - 1 - n); n++)
		;
	bars.right = (unsigned short)n;
	return bars;
}

#endif
