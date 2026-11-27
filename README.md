### Nintendont (orenatobr fork)

This is a fork of [FIX94/Nintendont](https://github.com/FIX94/Nintendont), not the original project. It follows
upstream commit `7a18345` and adds changes for the Wii U (vWii). Everything not listed here works as upstream
describes below. The full list, release by release, is in [CHANGELOG.md](CHANGELOG.md). Report problems with
this fork in this repository, not to the upstream authors.

#### What this fork changes
* **Force Widescreen fills the screen.** With Force Widescreen on and no Video Width chosen, the picture is
  688 pixels wide, centered, instead of the game's 640. A Video Width you set always wins. With Force Widescreen
  off, the 4:3 window comes from the Wii U Widescreen option turned off (game ini `AspectRatio = 2`): the Wii U
  centers a 4:3 picture with bars on the sides. A width under 640 pixels is not possible (the video hardware only
  scales up; it gave a green screen in `fork.16` and `fork.17`). A letterboxed video mode table (for example 448 lines from line 16) is
  also stretched to all 480 lines. The video hardware cannot stretch vertically by itself, so black bars drawn
  inside a game's own picture (for example with a widescreen Gecko code) are not removed by this.
* **Releases, and what is confirmed on a console.** The Latest is `v6.510-fork.18`. Confirmed by the owner on a
  Wii U: Super Mario Sunshine fills the whole screen with Force Widescreen on (since `fork.8`), Sunshine shows a
  centered 4:3 picture with Wii U Widescreen off (`AspectRatio = 2`), and Tales of Symphonia fills the whole screen
  (since `fork.15`, no setting needed). Not tried on a console yet: Sunshine with Force Widescreen on the `fork.18`
  build, and games other than these two. A game ini can set `VideoScale = 0` (or `Auto`) to undo the Video Width
  saved in the menu. `v6.510-fork.16` and `v6.510-fork.17` are pre-releases with the 540 pixel window (the latter
  gives a green screen with `VideoScale = 0` and Wii U Widescreen on): do not use them. `v6.510-fork.3` and
  `v6.510-fork.4` crash a Wii U when XFB Scan is on; do not use them. Rule of this fork: a build that touches the PPC hook or the
  video registers is a pre-release until a console run confirms it.
* **Why the bars appeared.** Sunshine runs a 448 line mode built by its own code; Tales of Symphonia gets its
  448 line mode from the SDK function `GXAdjustForOverscan` with 16 lines taken off. The video interface cannot
  scale vertically, so the fork makes those games keep 480 lines (`kernel/Patch.c`, `common/include/Sunshine480.h`,
  `common/include/AdjustOverscan.h`).
* **Status of the generic stretch.** There is no general crop or scale of a game's picture. The fork fixes the cause
  per game family (see above), and XFB Scan (logging only) measures the real video mode of any game.
* **Black bar detection (experimental).** The settings menu has XFB Scan (off by default). It logs the black bars
  of the picture to `ndebug.log`. It is the first step of an automatic stretch: Force Widescreen on fills the
  whole screen, off keeps a 4:3 window with side bars only. See [CHANGELOG.md](CHANGELOG.md).
* **Settings for one game.** `/teconmoon/<id4>.ini` on the card holds a few keys of Dolphin's GameINI format:
  `AspectRatio`, `wideScreenHack` (Force Widescreen), `ProgressiveScan`, `SkipIPL`, `PatchPAL50`, `VideoMode`,
  `Deflicker`, `VideoScale` and `VideoOffset`. The full description is in `loader/source/GameSettings.h`.
* **PAL discs stay at 50 Hz** unless the game's own settings say otherwise, because 60 Hz delays the audio.
* **Cheats for games started from a WUP.** A `/codes/<game id>.gct` file on the card is loaded without ticking Cheats.
* **Black video switch.** The loader keeps the screen black while it changes video mode.
* **Video diagnostics.** `ndebug.log` lists each video mode table of the game, before and after patching.

The default branch is `main`. It is protected: changes go through a pull request that needs one approval.
`scripts/build_docker.sh` builds `loader/loader.dol` with the pinned toolchain inside Docker, and
`scripts/test_video_fill.sh` runs the host test of the Force Widescreen width rule.

None of the Wii U changes above were run on a Wii. Check the release notes for what was tried on a console.

### Nintendont
A Wii Homebrew Project to play GC Games on Wii and vWii on Wii U

### Features:
* Works on Wii and Wii U (in vWii mode)
* Full-speed loading from a USB device or an SD card.
* Loads 1:1 and shrunken .GCM/.ISO disc images.
* Loads games as extracted files (FST format)
* Loads CISO-format disc images. (uLoader CISO format)
* Memory card emulation
* Play audio via disc audio streaming
* Bluetooth controller support (Classic Controller (Pro), Wii U Pro Controller)
* HID controller support via USB
* Custom button layout when using HID controllers
* Cheat code support
* Changeable configuration of various settings
* Reset/Power off via button combo (R + Z + Start) (R + Z + B + D-Pad Down)
* Advanced video mode patching, force progressive and force 16:9 widescreen
* Auto boot from loader
* Disc switching
* Use the official Nintendo GameCube controller adapter
* BBA Emulation (see [BBA Emulation Readme](BBA_Readme.md))

### Features: (Wii only)
* Play retail discs
* Play backups from writable DVD media (Old Wii only)
* Use real memory cards
* GBA-Link cable
* WiiRd
* Allow use of the Nintendo GameCube Microphone

### What Nintendont will never support:
* Game Boy Player

### Experimental microphone support with memory card emulation
On an original Wii with GameCube memory-card ports, enable both **Memcard
Emulation** and **hybrid EXI** in the settings menu
to use a virtual memory card in Slot A and physical peripherals, such as the
GameCube microphone, in slot 2 (Slot B).

The hybrid option defaults to off, including for settings saved by older
versions. It has no effect unless memory card emulation is also enabled, and
does nothing on Wii U/vWii or for Triforce games.
While active, it does not load or expose a virtual Slot B memory card; existing
Slot B save images are left untouched. Turn it off to restore the existing
virtual Slot B behavior.

Leave **Debugger** and **Debugger Wait** off when using a Slot B peripheral:
the USB Gecko debugger uses the same physical slot. With **Log** enabled, the
startup log reports `EXI: Hybrid EXI mode: virtual Slot A, physical Slot B.`

Slot B uses the game's own EXI routines on the real hardware, while Slot A and
EXI channel 2 stay emulated. The hardware interrupts of the emulated channels
stay masked, and the virtual Slot A interrupt status is only dispatched while a
virtual EXI interrupt is pending, so physical Slot B interrupts reach the game.

CPU-level regression checks are available after building. With Python and the
`unicorn` package installed, run:

```text
python scripts\test_hybrid_exi.py --devkitarm C:\path\to\devkitARM -v
```

These checks exercise the compiled ARM patch helpers, the settings upgrade, and
PowerPC routing and interrupt handlers in emulation, including all activation
flag combinations. They do not emulate the microphone or replace hardware
testing. Optionally add `--game-image D:\games\GP7E01\game.iso` to exercise
the USA Mario Party 7 interrupt-dispatch patch, native immediate-transfer
routine, and interrupt handlers against a local disc image. These game tests
are skipped when the image is omitted, missing, or not GP7E01. This test does not
save or distribute game code.

### Quick Installation:
1. Get the [loader.dol](loader/loader.dol?raw=true), rename it to boot.dol and put it in /apps/Nintendont/ along with the files [meta.xml](nintendont/meta.xml?raw=true) and [icon.png](nintendont/icon.png?raw=true).
2. Copy your GameCube games to the /games/ directory. Subdirectories are optional for 1-disc games in ISO/GCM and CISO format.
   * For 2-disc games, you should create a subdirectory /games/MYGAME/ (where MYGAME can be anything), then name disc 1 as "game.iso" and disc 2 as "disc2.iso".
   * For extracted FST, the FST must be located in a subdirectory, e.g. /games/FSTgame/sys/boot.bin .
3. Connect your storage device to your Wii or Wii U and start The Homebrew Channel.
4. Select Nintendont.

### Compiling:
For compile Nintendont yourself, get the following versions of the toolchain compiling PPC tools:
* **devkitARM r53-1**
* **devkitPPC r35-2**
* **libOGC 1.8.23-1**

These versions can be downloaded here: https://www.mediafire.com/folder/j0juqb5vvd6z5/devkitPro_archives

On Windows, run the "Build.bat" batch script for build Nintendont.

On Unix, run the "Build.sh" script.

Please use these specific versions for compiling Nintendont, **because if you try to compile them on latest dkARM/dkPPC/libOGC, you'll get a lot of compiler warnings and your build will crash when attemping to return to Nintendont menu**, so be warned about that.

### Notes
* The Wii and Wii U SD card slot is known to be slow. If you're using an SD card and are having performance issues, consider either using a USB SD reader or a USB hard drive.
* USB flash drives are known to be problematic.
* Nintendont runs best with storage devices formatted with 32 KB clusters. (Use either FAT32 or exFAT.)
