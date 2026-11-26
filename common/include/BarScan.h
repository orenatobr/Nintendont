#ifndef __BAR_SCAN_H__
#define __BAR_SCAN_H__

/*
 * Black bar detector for a YUY2 frame (the GameCube/Wii external framebuffer, XFB).
 * The Y of pixel x of a row is the byte at 2 * x, so only every second byte is read.
 * A pixel is black when Y <= NIN_BAR_BLACK_Y. A row or column belongs to a bar when at
 * least NIN_BAR_PERCENT percent of its pixels are black, so a little noise does not break a bar.
 * Bars are counted from each edge inwards, in pixels (left, right) and lines (top, bottom).
 * A frame that is black everywhere is reported as all_black, with every bar 0, because it
 * says nothing about bars. Limits: width <= NIN_BAR_MAX_W, height <= NIN_BAR_MAX_H.
 */
#define NIN_BAR_BLACK_Y		(0x18)
#define NIN_BAR_PERCENT		(98)
#define NIN_BAR_MAX_W		(720)
#define NIN_BAR_MAX_H		(576)

typedef struct NinBars
{
	unsigned short left;
	unsigned short right;
	unsigned short top;
	unsigned short bottom;
	unsigned char all_black;
} NinBars;

static inline NinBars NinScanBars(const unsigned char *xfb, unsigned width, unsigned height, unsigned stride)
{
	NinBars bars = {0, 0, 0, 0, 0};
	static unsigned short rowBlack[NIN_BAR_MAX_H];	/* static: the kernel stack is small, not reentrant */
	static unsigned short colBlack[NIN_BAR_MAX_W];
	unsigned x, y;
	if(width == 0 || height == 0 || width > NIN_BAR_MAX_W || height > NIN_BAR_MAX_H)
		return bars;
	for(x = 0; x < width; x++)
		colBlack[x] = 0;
	for(y = 0; y < height; y++)
	{
		const unsigned char *row = xfb + y * stride;
		unsigned count = 0;
		for(x = 0; x < width; x++)
		{
			if(row[2 * x] <= NIN_BAR_BLACK_Y)
			{
				count++;
				colBlack[x]++;
			}
		}
		rowBlack[y] = (unsigned short)count;
	}
	/* a row is in a bar when rowBlack * 100 >= width * PERCENT, a column when colBlack * 100 >= height * PERCENT */
	for(y = 0; y < height && (unsigned)rowBlack[y] * 100 >= width * NIN_BAR_PERCENT; y++)
		bars.top++;
	if(bars.top == height)
	{
		bars.top = 0;
		bars.all_black = 1;
		return bars;
	}
	for(y = height; y > 0 && (unsigned)rowBlack[y - 1] * 100 >= width * NIN_BAR_PERCENT; y--)
		bars.bottom++;
	for(x = 0; x < width && (unsigned)colBlack[x] * 100 >= height * NIN_BAR_PERCENT; x++)
		bars.left++;
	for(x = width; x > 0 && (unsigned)colBlack[x - 1] * 100 >= height * NIN_BAR_PERCENT; x--)
		bars.right++;
	return bars;
}

#endif
