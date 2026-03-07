# Minecraft Source Code Archives (2013-2014)

> [!WARNING]
> All files in this `archives/` directory are sourced from `https://hazbinhelluvageneral.com/cubevideojuego/cubegame/` for archival and preservation purposes only.

## Archive Source

**Primary Source:** https://hazbinhelluvageneral.com/cubevideojuego/cubegame/

This folder contains the original downloaded archives that were extracted to create the organized source code structure in the parent directories.

## Download Script

A batch script `download.bat` is provided to download all files from the source URL using curl.

### Usage
```batch
download.bat
```

This will:
1. Download all archive files from the source URL
2. Generate MD5 and SHA256 hashes for verification

## Hash Generation

To generate hashes for existing files, run:

```batch
generate-hashes.bat
```

This creates `hashes.txt` with all file checksums in CSV format.

## File Hashes

> [!NOTE]
> Hashes will be populated after running `download.bat` or manually downloading the files.

| File | MD5 | SHA256 |
|------|-----|--------|
| `mc-console-oct2014.zip` | *pending* | *pending* |
| `mc-cpp-snapshot.zip` | *pending* | *pending* |
| `minecraft.rar` | *pending* | *pending* |
| `MinecraftConsoles-141217.zip` | *pending* | *pending* |
| `MinecraftPS3Edition.7z` | *pending* | *pending* |
| `minecraft_dungeons.zip` | *pending* | *pending* |
| `minecraft_legends.zip` | *pending* | *pending* |
| `filelist.txt` | *pending* | *pending* |
| `Narration_NoGeneratedLevel_20170216.zip` | *pending* | *pending* |
| `enderman.zip` | *pending* | *pending* |
| `endercon.zip` | *pending* | *pending* |
| `linux-test.zip` | *pending* | *pending* |
| `patch.zip` | *pending* | *pending* |
| `info_storymode.txt` | *pending* | *pending* |

## Archive Contents Overview

### Console Edition Archives
| Archive | Extracted Location | Description |
|---------|-------------------|-------------|
| `mc-console-oct2014.zip` | `../console-edition-2014/oct-2014-backup/` | October 2014 backup snapshot |
| `MinecraftConsoles-141217.zip` | `../console-edition-2014/dec-2014-build/` | December 2014 build |
| `MinecraftPS3Edition.7z` | `../console-edition-2014/ps3-only-stripped/` | Initial PS3-only leak (stripped) |

### Pocket Edition Archives
| Archive | Extracted Location | Description |
|---------|-------------------|-------------|
| `mc-cpp-snapshot.zip` | `../pocket-edition-2013/mcpe-source/` | MCPE 0.6.1 C++ source (Feb 8, 2013) |

### Dungeons & Legends Archives
| Archive | Extracted Location | Description |
|---------|-------------------|-------------|
| `minecraft_dungeons.zip` | `../dungeons-legends/dungeons-source/` | Dungeons C++ source code |
| `minecraft_legends.zip` | `../dungeons-legends/legends-original/` | Legends original build |
| `Narration_NoGeneratedLevel_20170216.zip` | `../dungeons-legends/narration-build-2017/` | Dungeons narration build (2017) |

### Story Mode Archives
| Archive | Extracted Location | Description |
|---------|-------------------|-------------|
| `enderman.zip` | `../story-mode/enderman/` | Steam beta branch |
| `endercon.zip` | `../story-mode/endercon/` | Steam beta branch |
| `linux-test.zip` | `../story-mode/linux-test/` | Steam beta branch |
| `patch.zip` | `../story-mode/patch/` | Steam beta branch |

## Original Archive Sources (Pre-Extraction)

These archives were originally sourced from various locations before being mirrored:

| Original Archive | Source URL | Git Hash |
|------------------|------------|----------|
| `Minecraft.zip` | gofile.io/d/RRf1hv | - |
| `mc-console-oct2014.zip` | gofile.io/d/pxeEVD | 5ca1eff0124dd98f5bab76cfd2052dc8718b1c18 |
| `Minecraft PS3 Edition Source Code.zip` | gofile.io/d/NIawxk | - |
| `MinecraftConsoles-141217.zip` | gofile.io/d/pxeEVD | - |
| `mc-cpp-snapshot.zip` | gofile.io/d/pxeEVD | 2b519c7d6aacca7ce5865470e039268b43255217 |
| `Infdev_20140630_master.zip` | gofile.io/d/RONXEc | - |
| `Release.zip` | gofile.io/d/4srlJI | - |

## Lore

1. **First leak:** Anonymous user posted "Minecraft PS3 edition source code leak" - code was real but stripped all other platforms and modified file timestamps
2. **Second leak:** Another user leaked the actual full console edition source code dated 2014 with all platforms intact
3. **Third leak:** Yet another user posted additional leak containing MCPE

## Sources

- **Primary Archive Mirror:** https://hazbinhelluvageneral.com/cubevideojuego/cubegame/
- **Original 4chan Thread:** https://boards.4chan.org/g/thread/108264778/minecraft-ps3-edition-source-code
- **DMCA Information:** https://bin.skylvi.net/dmca

## DMCA Notice

> [!IMPORTANT]
> This content is for ARCHIVAL and PRESERVATIONAL purposes ONLY. Piracy is not condoned in any way.
>
> For information on DMCA takedown requests please read: https://bin.skylvi.net/dmca
