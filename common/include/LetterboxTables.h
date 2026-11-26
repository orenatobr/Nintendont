#ifndef __LETTERBOX_TABLES_H__
#define __LETTERBOX_TABLES_H__

/*
 * Video mode tables of 448 lines with a letterbox (viYOrigin 16) that Nintendont's table scanner does not
 * recognize because its filter bytes differ (Tales of Symphonia, seen in the "VI mode:" log). They are found
 * by the shape of their fields in an array of 32 bit words:
 *
 *   word 1: fbWidth 640 and efbHeight 448          0x028001C0
 *   word 2: xfbHeight 448, viXOrigin               0x01C0xxxx
 *   word 3: viYOrigin 16, viWidth 512 to 767       0x0010 02xx
 *   word 4: viHeight 448                           0x01C0xxxx
 *
 * Such a table is changed to xfbHeight 480, viYOrigin 0 and viHeight 480. efbHeight stays 448, so the game
 * scales its EFB copy by xfbHeight / efbHeight (the SDK default) and the picture fills the 480 lines. The
 * words are PPC (big endian) values, which the ARM kernel reads. Returns how many tables were changed.
 */
static inline unsigned NinPatchLetterboxTables(volatile unsigned *w, unsigned count)
{
	unsigned i, changed = 0;
	for(i = 0; i + 5 < count; i++)
	{
		if(w[i + 1] == 0x028001C0 && (w[i + 2] >> 16) == 0x01C0 && (w[i + 3] >> 16) == 0x0010
			&& (w[i + 3] & 0xFF00) == 0x0200 && (w[i + 4] >> 16) == 0x01C0)
		{
			w[i + 2] = (w[i + 2] & 0xFFFF) | (0x01E0u << 16);
			w[i + 3] = w[i + 3] & 0xFFFF;
			w[i + 4] = (w[i + 4] & 0xFFFF) | (0x01E0u << 16);
			changed++;
			i += 4;
		}
	}
	return changed;
}

#endif
