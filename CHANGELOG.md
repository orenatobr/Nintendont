# Changelog

All notable changes of this fork are listed here. The fork follows
[FIX94/Nintendont](https://github.com/FIX94/Nintendont) at commit `7a18345`; changes before that are in
[NEWS.md](NEWS.md) and the upstream history. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Fixed
- The 4:3 window without Force Widescreen. The owner's test of Super Mario Sunshine with Force Widescreen off and
  Wii U Widescreen on showed a picture stretched over the whole 16:9 screen (640 of 720 pixels, anamorphic), not a
  4:3 window. Now, with Wii U Widescreen on, Force Widescreen off and no Video Width chosen, the width is 540 of
  720 pixels (`NIN_WINDOW43_VIDEO_SCALE`, -60), centered at x origin 90, so the bars are on the sides only.
  Force Widescreen (688) and a chosen Video Width (640 to 720) still win. The offset limit follows the width
  (`NinMaxXOrigin`). Host test `scripts/test_video_fill.sh`. A game that draws its own 16:9 picture without Force
  Widescreen would look squeezed; set `VideoScale = 40` for it. Not tried on a console.

## [6.510-fork.15] - 2026-11-27

Confirmed on a Wii U by the owner: Tales of Symphonia fills the whole screen. The log of the run: one
`GXAdjustForOverscan` candidate (`0x00113828`), `ver is 0 now`, and the game mode went from `efb=448 xfb=448
yorig=16 h=448` to `efb=480 xfb=480 yorig=0 w=688 h=480` (`VTR` 480 lines). This release is the Latest. It also holds
everything from fork.9 to fork.14 (the entries below): the Sunshine 480 line patch without Force Widescreen (the 4:3
window, not tried on a console), the real video mode log, and the static letterbox table rule. Not tried on a
console: Sunshine on this build, Sunshine without Force Widescreen, and other games.

### Added
- `GXAdjustForOverscan` fix for games that build a 448 line letterbox mode from a 480 line one. The mode of
  Tales of Symphonia in the console log (`efb=448 xfb=448 yorig=16 h=448` out of 480 line tables, no 448 anywhere
  in the game) is what the SDK function `GXAdjustForOverscan(rmin, rmout, hor, ver)` gives with `ver = 16`. The
  kernel finds that function by its shape (`NinFindOverscanFuncs`, `common/include/AdjustOverscan.h`, host test
  `scripts/test_adjust_overscan.sh`) and a hook (`kernel/asm/VIAdjustHook.S`) sets `ver` to 0, so the mode keeps 480
  lines and everything built from it stays consistent. Log lines `Patch:[GXAdjustForOverscan]`, with the near
  misses listed when there is no single match. Applies to every game, with or without Force Widescreen.
  Experimental, not tried on a console; if the game draws at a fixed 448 lines the bottom may stay black.

### Added
- The mode reference search also looks for accesses through the small data register r13 (`ModeRef13:` lines):
  it finds the value of r13 where the game sets it (`lis r13` plus `addi` or `ori`) and lists the instructions
  that use r13 with an offset inside the video mode. The first search of Tales of Symphonia
  (6.510-fork.13) found 0 references through `lis`, which points to a global reached as `offset(r13)`.

### Added
- With XFB Scan on, when the kernel sees a new 640 pixel video mode, it searches the game code for the
  instructions that reach that mode (`lis rD,hi(mode)` followed by a load, store or add with that base register
  and an offset inside the mode) and logs them (`ModeRef:` lines), to find the code that fills a mode. The
  Tales of Symphonia log (6.510-fork.12) had no 640 by 448 word and no 448 stored by `sth`, so its 448 line
  mode at `0x802CAC8C` is computed.

### Added
- With XFB Scan on, the kernel also lists every word that is `fbWidth 640, efbHeight 448` with the words around
  it (`Tmpl448:` lines), to see the real fields of a 448 line mode table whatever they are. The `Mode448:` list of
  Tales of Symphonia (6.510-fork.11) was empty and its mode at `0x2CAC8C` lies inside the data that the game
  file fills, so it is a static table that the `NinPatchLetterboxTables` shape missed.

### Added
- With XFB Scan on, the kernel also lists the 448s that are stored into the efbHeight, xfbHeight or viHeight
  field of a video mode (`Mode448:` lines: `li rD,448` followed by `sth rD,6|8|0x10(rX)` within 8 words), to find
  the code that builds a run time mode. The console log of Tales of Symphonia (6.510-fork.10) showed its 448
  line mode in memory the file does not fill (built at run time), a game drawn at 640x448 everywhere, and two
  XFB buffers exactly 448 lines apart, so the height has to be changed where the mode is built.

### Added
- 448 line letterbox video tables that the table scanner does not recognize are found by the shape of their
  fields (`NinPatchLetterboxTables`, `common/include/LetterboxTables.h`, `scripts/test_letterbox_tables.sh`) and
  changed to 480 lines, for any game, with or without Force Widescreen (log line `Patch:[448 line tables]`). The
  console log of Tales of Symphonia showed such a table: `efb=448 xfb=448 yorig=16 h=448` at `0x802CAC8C`, next
  to four standard 480 line tables that were already handled. efbHeight stays 448, so the game scales its EFB
  copy by xfbHeight / efbHeight. Experimental, not tried on a console.

### Status (2026-11-26)
- Latest is `v6.510-fork.8`. fork.9 to fork.13 are pre-releases (diagnostics, the Sunshine 4:3 window path and the
  static letterbox table rule). The 4:3 window (Force Widescreen off) is not tried on a console. Tales of Symphonia
  is a follow-up with a stopping rule: if the `ModeRef` log does not show the code that builds its 448 line mode,
  it stays documented and does not block the Sunshine work.
- Source consulted: the [Dolphin wiki page of Super Mario Sunshine](https://wiki.dolphin-emu.org/index.php?title=Super_Mario_Sunshine)
  (section 2.8) gives the 16:9 Gecko codes, which only change aspect constants and HUD positions. It says nothing
  about the video mode, which is why the cause of the bars was found with the console logs, not from the wiki.
- Confirmed on a console by the owner with `v6.510-fork.8`: Super Mario Sunshine fills 100% of the screen.
- Process rule since fork.3 and fork.4 crashed a Wii U: a build that touches the PPC hook or the video registers is
  a pre-release until a console run confirms it, then it becomes Latest. fork.3 and fork.4 are pre-releases now.
- Not generalized yet: Tales of Symphonia still has thin horizontal bars. The real video mode of any game can be
  logged with XFB Scan (`VI mode:` lines, `v6.510-fork.9` pre-release).

### Added
- With XFB Scan on, the VIConfigure hook also leaves the video mode pointer of the game in shared memory and the
  kernel logs the real mode (`VI mode: ... efb= xfb= xorig= yorig= w= h=`), which shows modes built at run time
  in any game. The hook is installed when XFB Scan is on even if no width is wanted.

### Changed
- The vertical fill of letterboxed static tables (448 lines from line 16 to 480 from line 0) no longer needs
  Force Widescreen; it follows the rule for Sunshine (a full height picture in both modes).

### Changed
- The Super Mario Sunshine 448 to 480 line patch no longer needs Force Widescreen. Without Force Widescreen the
  game now also fills the 480 lines (no bars above and below) and keeps its own 4:3 window width, so only
  bars at the sides remain; with Force Widescreen on the width fills 688 pixels as before.

### Added
- A hook at the start of the game's `VIConfigure` (`kernel/asm/VIWidthHook.S`, applied in `kernel/Patch.c`; log
  line `Patch:[VIConfigure width hook]`) sets the picture width and origin of a video mode the game builds at
  run time, with the same rule as the static tables (the Video Width in use: the one chosen, or 688 with Force
  Widescreen on, and the Video Offset). Found by the console log of fork.7: Super Mario Sunshine output width
  came out near 658 pixels (`HSR=0x10f9`), not the 688 the tables set, so thin side bars stayed. Only the video
  output reads those two fields. Experimental, not tried on a console.

### Added
- Super Mario Sunshine, Force Widescreen on: the 448 line video mode is changed to 480 lines (`NinPatch448Lines` in
  `common/include/Sunshine480.h`, applied in `kernel/Patch.c`; log line `Patch:[Sunshine 480 lines] N site(s)`).
  It changes the 448 in the game's width/height getter functions and in its NTSC height branches, found by their
  exact instruction shape from the `Scan448:` log of the console (17 uses of 448 in the game, 7 match). The other
  uses are not touched. `scripts/test_sunshine_480.sh` tests the matcher with words from that log. Experimental:
  not tried on a console, and the game's own viewport may still draw 448 lines.

### Found
- First console log of XFB Scan (6.510-fork.5, Super Mario Sunshine): the video registers and the XFB read are
  right, and the game runs a **448 line** video mode (`VTR` active lines `0x1C0`; the XFB has 448 valid lines
  and 32 empty ones, `bottom=32`). Its static tables are all 480 lines, so the mode is built at run time. The
  bars are the 32 missing lines of the 480 line signal; the video interface cannot scale vertically, so the
  fix has to make the game draw and copy 480 lines.

### Added
- With XFB Scan on, the kernel lists where a game loads the number 448 (`Scan448:` lines in `ndebug.log`),
  to find where Super Mario Sunshine builds its 448 line mode. Diagnostic only.

### Added
- `NinPlanStretch` (`common/include/StretchPlan.h`) with `scripts/test_stretch_plan.sh`: from the bars found, it
  plans what to show and where. Force Widescreen on cuts every bar and fills the screen (688 px, origin 16);
  off keeps a 4:3 window (bars above and below cut, side bars kept, 640 px at origin 40). Dark scenes and
  bars that leave less than half of the picture are not trusted. It is not connected to the console yet.

### Status of the stretch (what exists and what does not)
- Exists: the bar detector, the XFB Scan log (6.510-fork.5, not tried on a console yet), the width rule for
  Force Widescreen (688 px) and the letterbox table rule (448 to 480 lines), and the stretch plan above (host
  tests only).
- Does not exist: the stretch itself. Nothing crops or scales the picture of a game yet, so the black bars of
  Super Mario Sunshine are still there. Phase 2 (side bars, through the video interface) and phase 3 (top and
  bottom bars, through the EFB copy) need the XFB Scan log from a console first.
- The video register layout used by XFB Scan matches libogc `video.c`: base `0xCC002000`, top field base in
  registers 14 and 15 (bytes `0x1C` and `0x1E`), bit 28 set means the address is stored shifted right by 5,
  `PICCONF` at `0x48` and `HSR` at `0x4A`.

## [6.510-fork.5] - 2026-11-26

### Fixed
- XFB Scan crashed the Wii U to a black screen (6.510-fork.3 and 6.510-fork.4): the kernel (ARM) read the video
  registers at an assumed address `0x0d002000`, which is not valid for it. The PPC hook of the game
  (`PADReadGC.c`) now reads the video registers, where `0xCC002000` is real, and leaves the XFB address and the
  raw registers at `0x13003F00`. The kernel reads the XFB from main memory, which is safe, measures the bars and
  writes the log line. The detector (`NinScanBars`) no longer needs work buffers: it scans from the edges
  inwards and stops at the first row or column that is not black. The first attempt to measure inside the hook
  did not fit: that hook has only 12 KB.

## [6.510-fork.4] - 2026-11-26

### Added
- `XfbScan = True` (or `False`) in a game's `/teconmoon/<id4>.ini` switches the experimental XFB Scan on or
  off for that game. A game started from a WUP ignores the settings menu, so this is the only way to use the
  scan there. `scripts/test_gamesettings.sh` covers the parser.

## [6.510-fork.3] - 2026-11-26

### Added
- Automatic black bar detection, phase 1 of 3. A new setting, **XFB Scan (exp.)** (right column of the settings
  menu, off by default), makes the kernel look at the picture the game shows every 2 seconds and write the bars
  it finds to `ndebug.log` (`XFB scan: xfb=... left=.. right=.. top=.. bottom=..`, with the raw video
  registers). It changes nothing on screen and needs Log on. The detector is `NinScanBars` in
  `common/include/BarScan.h`, covered by `scripts/test_bar_scan.sh`.
- The video registers are read at `0x0d002000`, which is an assumption (the Flipper registers as the Starlet
  sees them). The raw registers are logged so that it can be checked; a line `no usable XFB` means it was wrong.

### Planned (target behavior, from the owner)
- Force Widescreen on in the game's settings: stretch the picture over the whole screen, horizontally and
  vertically, using the detected bars (phases 2 and 3).
- Force Widescreen off: stretch a 4:3 window with no bars above and below, only at the sides.
- Phase 2 crops side bars and scales through the horizontal scaler of the video interface. Phase 3 changes the
  EFB copy for top and bottom bars, because the video interface cannot scale vertically.

## [6.510-fork.2] - 2026-11-26

### Added
- Force Widescreen also fills the height of a letterboxed video mode table: a table whose picture is
  `xfbHeight == viHeight` lines, fewer than 480, starting below line 0 (for example 448 lines from line 16) is
  changed to 480 lines from line 0 (`NinFillVertical` in `common/include/CommonConfig.h`, applied in
  `kernel/Patch.c`, logged as `filled vertically` in `ndebug.log`). The game scales its EFB copy by
  `xfbHeight / efbHeight`, so the picture grows to the full height. Tables that already use 480 lines or more
  are left alone.

### Known limits
- Super Mario Sunshine's static tables already use 480 lines, so this rule changes nothing for them. It helps
  only if the game's table is letterboxed. Bars the game draws inside its own picture are still not removed.
- Not tried on a console when this entry was written.

## [6.510-fork.1] - 2026-11-26

### Added
- Force Widescreen fills the screen. With it on and no Video Width chosen, the game's video mode tables are
  patched to 688 pixels wide, centered, and the loader uses the same width (`NinEffectiveVideoScale` in
  `common/include/CommonConfig.h`). A Video Width set in the menu or in the game's ini always wins; with Force
  Widescreen off the game keeps its own 4:3 window. 688 is the widest width that works on the Wii U: 720 gave a
  black screen on Super Mario Sunshine.
- `scripts/test_video_fill.sh`, a host test of that width rule, and `scripts/build_docker.sh`, which builds
  `loader/loader.dol` with the pinned toolchain (devkitARM r53-1, devkitPPC r35-1, libOGC 1.8.23-1) in Docker.
- The fork changes brought in from the `teconmoon` branch (pull request #1):
  - Per-game settings in `/teconmoon/<id4>.ini` (aspect, Force Widescreen, Force Progressive, Skip IPL,
    Patch PAL50, video mode, deflicker, Video Width, Video Offset).
  - Cheats load for games started from a WUP when `/codes/<game id>.gct` exists.
  - `ndebug.log` lists each video mode table before and after patching.

### Changed
- The default branch is `main` instead of `master`, protected by a pull request that needs one approval.
- PAL discs stay at 50 Hz unless the game's own settings say otherwise.
- The loader keeps the screen black while it switches to the game's video mode.
- README describes the fork and its differences.

### Known limits
- The video hardware has no vertical scaler. Black bars that a game draws inside its own picture, such as the
  ones Super Mario Sunshine shows with a widescreen Gecko code, are not removed by the width change. The
  static video tables of Super Mario Sunshine already fill all 480 lines (`ndebug.log`, `Patch:VI table`).
- Not tried on a console when this entry was written.

[Unreleased]: https://github.com/orenatobr/Nintendont/compare/v6.510-fork.5...HEAD
[6.510-fork.5]: https://github.com/orenatobr/Nintendont/compare/v6.510-fork.4...v6.510-fork.5
[6.510-fork.4]: https://github.com/orenatobr/Nintendont/compare/v6.510-fork.3...v6.510-fork.4
[6.510-fork.3]: https://github.com/orenatobr/Nintendont/compare/v6.510-fork.2...v6.510-fork.3
[6.510-fork.2]: https://github.com/orenatobr/Nintendont/compare/v6.510-fork.1...v6.510-fork.2
[6.510-fork.1]: https://github.com/orenatobr/Nintendont/releases/tag/v6.510-fork.1
