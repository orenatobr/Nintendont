# Changelog

All notable changes of this fork are listed here. The fork follows
[FIX94/Nintendont](https://github.com/FIX94/Nintendont) at commit `7a18345`; changes before that are in
[NEWS.md](NEWS.md) and the upstream history. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

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
