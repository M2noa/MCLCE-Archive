# Minecraft Source Code Archives (2013-2014)

> [!WARNING]
> All files in this `archives/` directory are sourced from `https://hazbinhelluvageneral.com/cubevideojuego/cubegame/` for archival and preservation purposes only.

> [!IMPORTANT]
> **GitHub File Size Limitation:** Files larger than 2GB cannot be stored on GitHub even with Git LFS. The following files exceed this limit and must be downloaded from the source:
> - `patch.zip` (4.2 GB)
> - `enderman.zip` (3.8 GB)
> - `linux-test.zip` (3.8 GB)
> - `endercon.zip` (3.2 GB)
> - `mc-console-oct2014.zip` (2.2 GB)
> - `minecraft.rar` (2.2 GB)
>
> Use the `download.bat` script to download all files including those above the GitHub limit.

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
> Generated: 2026-03-07

| File | Size | MD5 | SHA256 |
|------|------|-----|--------|
| `mc-console-oct2014.zip` | 2.2 GB | `7132eb8132a66b1ca362177afdb06ae0` | `f0fd83f3fe2693dbcd53336e57f5d1823232723b6c45583e3c8898ee358687cd` |
| `mc-cpp-snapshot.zip` | 20.6 MB | `4e13c237b6191f5a6cb18cdd0a920248` | `26edfab4681eee29bd65028dd24c093e87d230aa343f282e1ab9be4f0e6331fb` |
| `minecraft.rar` | 2.2 GB | `66ecffa7d23adb20476a4c8c24757692` | `7b250bb3b4c8358e028f54dfa28dea7b99558ae202cfb047232524801dcec45c` |
| `MinecraftConsoles-141217.zip` | 496 MB | `2afe963d2900c997cdcb4ca744af9498` | `9f5dfdfaab574df6798959afefe483546bedc0ba463e8f40a4d6102af297229b` |
| `MinecraftPS3Edition.7z` | 573 MB | `7f6b84b3fefb089ab80257cf1c6805d1` | `d3c6449dc38d8579f7a799689bec50fdfb3cdc5b105761dda61e1bcef028077f` |
| `minecraft_dungeons.zip` | 156 MB | `4c051a49abc16af3be8f0ab824f5ee2b` | `dd279cf738a16f78c7d733e5fb213426857cc2a4855cfad06ec40d3ec464efb0` |
| `minecraft_legends.zip` | 160 MB | `7710c2f9fee1a36eec4f895a4484b8cd` | `f70a989e022143e00a3d2f42cc07c192357fe46b3b1f58cd94a20511ab6d0a7d` |
| `Narration_NoGeneratedLevel_20170216.zip` | 207 MB | `a9612bb2de0e94a0f4fa53df13f5cb69` | `8335fc69be80cd340730473a0923a3e5891ab34e61297b8735bb286a4c4b0c93` |
| `enderman.zip` | 3.8 GB | `195390cac9cd14706941c7b9fad194e6` | `fe0773a49791041cd01e14a2cfdf5c4506fc71a473530bffd59572066aed048a` |
| `endercon.zip` | 3.2 GB | `5b0e21623c2790ac6c253fa2c3585bbb` | `9f130c6142e6945fdd61664d70768dfe87dd8f93b60ad188936a02d220b3c5be` |
| `linux-test.zip` | 3.8 GB | `64302c1c4fdb3a444968a664844156c9` | `4b3fc3955023c3c6e239586579f55fff328ad9e0a05646dc0c8d461446c8bf18` |
| `patch.zip` | 4.2 GB | `a706df1fe8ce4a9422864933eec8b040` | `562413c94c7d7b276574548728d3bc92b2b057175672967ec8dbdab56e3b32e8` |
| `filelist.txt` | 508 KB | - | - |
| `info_storymode.txt` | 7.8 KB | - | - |

**Total Size:** ~20.9 GB

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
