/*
 * Host test for NinScanBars (common/include/BarScan.h), the black bar detector.
 * Run: scripts/test_bar_scan.sh
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "BarScan.h"

#define W 640
#define H 480

static unsigned char frame[W * 2 * H];

/* YUY2: Y of pixel x is at byte 2*x. Fill everything with picture (Y = 0x80), then black the given margins. */
static void make(unsigned left, unsigned right, unsigned top, unsigned bottom)
{
	unsigned x, y;
	for(y = 0; y < H; y++)
		for(x = 0; x < W; x++)
		{
			int black = x < left || x >= W - right || y < top || y >= H - bottom;
			frame[y * W * 2 + 2 * x] = black ? 0x10 : 0x80;
			frame[y * W * 2 + 2 * x + 1] = 0x80;
		}
}

static NinBars scan(void)
{
	return NinScanBars(frame, W, H, W * 2);
}

int main(void)
{
	NinBars b;
	/* a full picture has no bars */
	make(0, 0, 0, 0);
	b = scan();
	assert(b.left == 0 && b.right == 0 && b.top == 0 && b.bottom == 0 && !b.all_black);
	/* side bars (pillarbox) */
	make(80, 80, 0, 0);
	b = scan();
	assert(b.left == 80 && b.right == 80 && b.top == 0 && b.bottom == 0);
	/* top and bottom bars (letterbox) */
	make(0, 0, 32, 48);
	b = scan();
	assert(b.left == 0 && b.right == 0 && b.top == 32 && b.bottom == 48);
	/* bars on all four sides */
	make(40, 24, 16, 16);
	b = scan();
	assert(b.left == 40 && b.right == 24 && b.top == 16 && b.bottom == 16);
	/* a few bright pixels of noise inside a bar do not break it (98% rule) */
	make(80, 80, 0, 0);
	frame[10 * W * 2 + 2 * 5] = 0x80;
	b = scan();
	assert(b.left == 80);
	/* an all black frame is reported as such, not as four bars */
	make(W, 0, 0, 0);
	b = scan();
	assert(b.all_black && b.left == 0 && b.right == 0 && b.top == 0 && b.bottom == 0);
	/* a stride larger than the visible width (padding) is honored */
	{
		static unsigned char padded[(W + 16) * 2 * H];
		unsigned x, y;
		memset(padded, 0x80, sizeof(padded));
		for(y = 0; y < H; y++)
			for(x = 0; x < 50; x++)
				padded[y * (W + 16) * 2 + 2 * x] = 0x10;
		b = NinScanBars(padded, W, H, (W + 16) * 2);
		assert(b.left == 50 && b.right == 0);
	}
	puts("test_bar_scan: OK");
	return 0;
}
