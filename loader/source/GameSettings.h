#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

/*
 * Teconmoon: per game settings. /teconmoon/<game id>.ini holds a few keys of Dolphin's GameINI format.
 * Section names are ignored and every other line is skipped, so a Dolphin GameSettings file can be used as it is.
 *
 *   AspectRatio     = 1 (16:9) or 2 (4:3)   Wii U Widescreen on or off (0 or anything else: leave it)
 *   wideScreenHack  = True or False         Force Widescreen
 *   ProgressiveScan = True or False         Force Progressive
 *   SkipIPL         = True or False         Skip IPL
 *   PatchPAL50      = True or False         Patch PAL50 (Teconmoon key, Dolphin has none)
 *   VideoMode       = Auto, PAL50, PAL60, NTSC or MPAL   force that video mode, or leave it to the game (Teconmoon key)
 *   Deflicker       = True or False         with a forced VideoMode: the GX deflicker filter on or off (Teconmoon key)
 *
 *   VideoScale      = 40 to 120 or 0/Auto   horizontal size of the picture: 640 + the value - 40 pixels of the 720 wide signal (Teconmoon key; the menu's Video Width).
 *                                           0 or Auto undoes a Video Width set in the menu, so Force Widescreen fills the screen (688 pixels) and the game keeps its own 640 pixel table
 *   VideoOffset     = -20 to 20             moves the picture left (negative) or right (Teconmoon key; the menu's Video Offset)
 *   XfbScan         = True or False         the experimental XFB Scan: logs the black bars of the picture to ndebug.log (Teconmoon key)
 *
 * ProgressiveScan = True, PatchPAL50 = True and a forced VideoMode other than Auto also switch off the PAL guard of this loader for that
 * game: it is the user's choice. VideoMode = PAL50 keeps a PAL game at 50 Hz, so with Deflicker = False it only turns the filter off.
 */
enum GameSetting
{
	GS_WIIU_WIDE,
	GS_FORCE_WIDE,
	GS_FORCE_PROG,
	GS_SKIP_IPL,
	GS_PATCH_PAL50,
	GS_VIDEO_MODE,		/* 0 Auto, 1 PAL50, 2 PAL60, 3 NTSC, 4 MPAL */
	GS_DEFLICKER,
	GS_VIDEO_SCALE,		/* 40 to 120 as is, 0 for Auto */
	GS_VIDEO_OFFSET,	/* -20 to 20, stored as the value + 64 */
	GS_XFB_SCAN,		/* True or False: the experimental XFB Scan (bar detection log) */
	GS_COUNT
};

/* Reads the text (modified in place) and sets out[i] to 1 (on), 0 (off) or -1 (not mentioned). Returns how many are set. */
int GameSettingsParse(char *text, signed char out[GS_COUNT]);

#endif
