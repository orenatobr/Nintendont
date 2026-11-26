# Changelog

All notable changes of this fork are listed here. The fork follows
[FIX94/Nintendont](https://github.com/FIX94/Nintendont) at commit `7a18345`; changes before that are in
[NEWS.md](NEWS.md) and the upstream history. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

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

[Unreleased]: https://github.com/orenatobr/Nintendont/compare/v6.510-fork.2...HEAD
[6.510-fork.2]: https://github.com/orenatobr/Nintendont/compare/v6.510-fork.1...v6.510-fork.2
[6.510-fork.1]: https://github.com/orenatobr/Nintendont/releases/tag/v6.510-fork.1
