# Minecraft Source Code Archive - Download Sources & Verification

This document maps all archived content to their original download sources.

## Download Sources

### Console Edition (2014)

| Archive | Extracted To | Source | Notes |
|---------|--------------|--------|-------|
| `Minecraft.zip` | `console-edition-2014/full-multiplatform/` | https://bin.skylvi.net/Leaks/Minecraft/ | Full multi-platform console source |
| `mc-console-oct2014.zip` | `console-edition-2014/oct-2014-backup/` | https://bin.skylvi.net/Leaks/Minecraft/ | October 2014 backup snapshot |
| `Minecraft PS3 Edition Source Code.zip` | `console-edition-2014/ps3-only-stripped/` | https://bin.skylvi.net/Leaks/Minecraft/ | Initial PS3-only leak (stripped) |
| `MinecraftConsoles-141217.zip` | `console-edition-2014/dec-2014-build/` | https://bin.skylvi.net/Leaks/Minecraft/ | December 2014 build |

### Pocket Edition (2013)

| Archive | Extracted To | Source | Notes |
|---------|--------------|--------|-------|
| `mc-cpp-snapshot.zip` | `pocket-edition-2013/mcpe-source/` | https://bin.skylvi.net/Leaks/Minecraft/ | MCPE 0.6.1 C++ source (Android, iOS, Raspberry Pi) |

### Infdev (2014)

| Archive | Extracted To | Source | Notes |
|---------|--------------|--------|-------|
| `Infdev_20140630_master.zip` | `infdev-2014/source/` | https://bin.skylvi.net/Leaks/Minecraft/ | PC Infdev custom engine |

### Pre-compiled Windows

| Archive | Extracted To | Source | Notes |
|---------|--------------|--------|-------|
| `Release.zip` | `precompiled-windows/` | https://bin.skylvi.net/Leaks/Minecraft/ | Windows x86_64 executable + assets |

### Dungeons & Legends

| Archive | Extracted To | Source | Notes |
|---------|--------------|--------|-------|
| `spice-5f6757b2078527772785f68eece55d9a7da29731.zip` | `dungeons-legends/dungeons-source/` | https://drive.proton.me/urls/EVM0GRC0MM#WckH5oSa6fOn | Dungeons C++ source code |
| `minecraft_dungeons.zip` | `dungeons-legends/dungeons-original/` | https://hazbinhelluvageneral.com/cubevideojuego/cubegame | Original Dungeons build |
| `minecraft_legends.zip` | `dungeons-legends/legends-original/` | https://archive.org/details/minecraft-console-legacy-edition-source-code-leak | Legends beta based on MCBE 1.2.9.0 Win32 Dev with .PDB |
| `Narration_NoGeneratedLevel_20170216.zip` | `dungeons-legends/narration-build-2017/` | https://hazbinhelluvageneral.com/cubevideojuego/cubegame | Dungeons dev build based on MCBE 0.15.0.0 (by MDLC) |
| `filelist.txt` | `dungeons-legends/filelist.txt` | https://hazbinhelluvageneral.com/cubevideojuego/cubegame | File listing |

### Story Mode

| Archive | Extracted To | Source | Notes |
|---------|--------------|--------|-------|
| `info_storymode.txt` | `story-mode/info_storymode.txt` | https://hazbinhelluvageneral.com/cubevideojuego/cubegame | Beta branch passwords |
| `enderman.zip` | `story-mode/enderman/` | https://hazbinhelluvageneral.com/cubevideojuego/cubegame | Steam beta branch (Steam-cracked) |
| `endercon.zip` | `story-mode/endercon/` | https://hazbinhelluvageneral.com/cubevideojuego/cubegame | Steam beta branch (Steam-cracked) |
| `linux-test.zip` | `story-mode/linux-test/` | https://hazbinhelluvageneral.com/cubevideojuego/cubegame | Steam beta branch (Steam-cracked) |
| `patch.zip` | `story-mode/patch/` | https://hazbinhelluvageneral.com/cubevideojuego/cubegame | Steam beta branch (Steam-cracked) |

### Additional Sources (Not Yet Obtained)

| Archive | Source | Notes |
|---------|--------|-------|
| `mcsm.zip` | https://x.com/coah80/status/2028925081288638597 | Story Mode beta builds (Steam-cracked, no Steam needed) |
| `Win32.Netease.OGL.Publish_2.2.2.44626_20180717111056.7z` | https://t.me/MDLC_main | Chinese Netease v1.5.1 with .PDB debug symbols |
| `MinecraftConsoles-backups-oct_10_2014.rar` | https://t.me/MDLC_main | Console Edition backup (split archive, 2GB+) |

## Archive Verification

MD5 hashes are for original archive files, not extracted folders. Use file counts and sizes for verification of extracted content.

## Git Hashes (Where Available)

- `mc-cpp-snapshot.zip`: `2b519c7d6aacca7ce5865470e039268b43255217`
- `mc-console-oct2014.zip`: `5ca1eff0124dd98f5bab76cfd2052dc8718b1c18`
- `spice-5f6757b2078527772785f68eece55d9a7da29731.zip`: `5f6757b2078527772785f68eece55d9a7da29731`

## Archive Passwords

- `minecraft.rar`: `fuckanon`
- `MinecraftPS3Edition.7z`: `U2FsdGVkX187GOHqhIryMT+tJgiOcwSNH6UkWAw80Y37xpUsp40tC/+59LY6DIqm7G8+9y+44PIfqmVl8lnb72rhmZKN/UWN7J1JMPXlJ8I=`

## Technical Notes

### Console Edition (mc-console-oct2014)
- Java-like build on C++ (not Bedrock-based)
- Split archive due to Telegram's 2GB file upload limit
- Original source: https://t.me/MDLC_main

### Pocket Edition (mc-cpp-snapshot)
- Version: 0.6.1
- C++ source code for mobile platforms

### Dungeons Narration Build
- Based on MCBE 0.15.0.0 core
- Can be used as regular MCBE (not Dungeons mode)
- Ctrl + E switches between Dungeons and regular mode
- First-person view available like MCBE

### Legends Build
- Based on MCBE 1.2.9.0 Win32 Development
- Includes .PDB debug symbols and function names
- Can be used as regular game with custom world
- Requires opening badger world before regular worlds
- World storage: `%AppData%\MinecraftPE\games\com.mojang\minecraftWorlds`

### Chinese Netease Build (Not Yet Obtained)
- Version: 1.5.1 with debug information
- Includes .PDB with function names
- Requires special `mc.cfg` file to run
- EXE must be decrypted through "Unlicense tool" or "Magicmida" before using with .PDB in IDA PRO
- Useful for reverse engineering

### Story Mode Builds (mcsm.zip)
- Modified to not require Steam
- Cracked using GitHub tool

## Wayback Machine Archives

All files from hazbinhelluvageneral.com are archived at:
https://web.archive.org/web/20260304065837/https://hazbinhelluvageneral.com/cubevideojuego/cubegame

## Original Leak Information

### Leak Timeline
1. **First leak (2014)**: PS3-only source code (stripped, modified timestamps)
2. **Second leak (2014)**: Full console edition multi-platform source
3. **Third leak (2014)**: MCPE (Pocket Edition) source code
4. **Fourth leak (2017-2021)**: Dungeons & Legends builds
5. **Fifth leak (2024+)**: Story Mode beta branches

### Original 4chan Thread
https://boards.4chan.org/g/thread/108264778/minecraft-ps3-edition-source-code

### DMCA Information
https://bin.skylvi.net/dmca

---

**Last Updated:** 2026-03-04  
**Repository Status:** Complete - All available builds archived
