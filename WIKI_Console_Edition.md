# Console Edition (2014)

Complete multi-platform Console Edition source code from 2014.

## Overview

| Attribute | Value |
|-----------|-------|
| **Platforms** | PS3, PS4 (Orbis), PS Vita, Xbox 360, Xbox One (Durango), Windows x86_64 |
| **Total Size** | ~3 GB |
| **Files** | 19,433+ |
| **Language** | C++ |
| **Year** | 2014 |
| **Build System** | Java-like on C++ (not Bedrock-based) |

---

## Variants

### full-multiplatform/

**Source**: `Minecraft.zip` (gofile.io/d/RRf1hv)  
**Date**: 2014  
**Platforms**: All (PS3, PS4, PS Vita, Xbox 360, Xbox One, Windows)

Complete multi-platform console edition source code. Contains platform-specific folders for all supported consoles.

**Platform Structure**:
```
full-multiplatform/
├── PS3/              # PlayStation 3
├── Orbis/            # PlayStation 4
├── OrbisMedia/       # PS4 media assets
├── PSVita/           # PlayStation Vita
├── PSVitaMedia/      # PS Vita media assets
├── Xbox/             # Xbox 360
├── Durango/          # Xbox One
├── DurangoMedia/     # Xbox One media assets
├── Windows64/        # Windows 64-bit
├── Windows64Media/   # Windows media assets
└── Common/           # Shared code across platforms
```

---

### oct-2014-backup/

**Source**: `mc-console-oct2014.zip` (gofile.io/d/pxeEVD)  
**Date**: October 10, 2014  
**Git Hash**: `5ca1eff0124dd98f5bab76cfd2052dc8718b1c18`

Backup snapshot from October 2014. Similar structure to full-multiplatform.

---

### ps3-only-stripped/

**Source**: `Minecraft PS3 Edition Source Code.zip` (gofile.io/d/NIawxk)  
**Date**: 2014 (modified timestamps)  
**Platforms**: PS3 only

> **⚠️ Warning**: Initial leak - PS3 edition with other platforms stripped out and file timestamps modified.

This was the first leak before the full multi-platform version was released.

---

### dec-2014-build/

**Source**: `MinecraftConsoles-141217.zip` (gofile.io/d/pxeEVD)  
**Date**: December 17, 2014

Console edition build from December 2014. Contains Visual Studio solution file.

---

### whisper-lce/ ⭐

**Modified By**: M1noa  
**Date**: March 2026  
**Status**: Active Development

M1noa's modified version of Console Edition with bug fixes, enhancements, and PC-specific features.

**Key Features**:
- 3 critical bug fixes (chest crash, void spawn, random damage)
- Increased entity limits (TNT, gravity blocks)
- Infinite world size (38× larger)
- Sign censoring removed
- Enhanced debug menu (F3)
- Chat system re-enabled

**Learn More**: [Whisper LCE](Whisper-LCE)

---

## Leak History

```
Timeline:

1st Leak (2014) ──► PS3-only (stripped, modified timestamps)
                    └─► "Minecraft PS3 edition source code leak"

2nd Leak (2014) ──► Full multi-platform source
                    └─► Actual complete console edition

3rd Leak (2014) ──► MCPE source code
                    └─► Pocket Edition 0.6.1 (see Pocket Edition)

Modified (2026) ──► Whisper LCE
                    └─► M1noa's enhanced version
```

---

## Build Requirements

> **⚠️ Important**: Building requires Visual Studio 2012/2022 and various proprietary libraries (Iggy, Miles, Sentient, 4JLibs).

### For Original Versions

- Visual Studio 2012 (v110 toolset)
- Platform SDKs (PS3, Xbox 360, etc.)
- Proprietary libraries (not included)

### For Whisper LCE

- Visual Studio 2022 (v143 toolset) or VS 2012 (v110)
- Windows 10/11 SDK
- 4JLibs (included in whisper-lce)

**See**: [Whisper LCE](Whisper-LCE) for detailed build instructions

---

## Launch Arguments

For Windows builds:

- `-name <playername>` - Set player name
- `-ip <address>` - Set server IP
- `-port <port>` - Set server port

---

## Technical Details

### Architecture

- **Language**: C++
- **Build System**: Java-like structure (not Bedrock-based)
- **Rendering**: Custom renderer (not DirectX/OpenGL directly)
- **Networking**: Custom protocol
- **Save Format**: Custom NBT-based format

### Platform Support

| Platform | Code Name | Architecture | Status |
|----------|-----------|--------------|--------|
| PlayStation 3 | PS3 | PowerPC | Original |
| PlayStation 4 | Orbis | x86_64 | Original |
| PS Vita | PSVita | ARM | Original |
| Xbox 360 | Xbox | PowerPC | Original |
| Xbox One | Durango | x86_64 | Original |
| Windows | Windows64 | x86_64 | Original + Whisper LCE |

---

## Version Information

**TU Version**: TU14 era (2014)  
**Minecraft Version**: ~1.04-1.05 (Console Edition versioning)  
**Java Equivalent**: ~1.6.4 - 1.8 features

---

## Sources

| Source | URL |
|--------|-----|
| Original Thread | https://boards.4chan.org/g/thread/108264778/minecraft-ps3-edition-source-code |
| DMCA Info | https://bin.skylvi.net/dmca |
| Archive Mirror | https://bin.skylvi.net/Leaks/Minecraft/ |

---

## Related Pages

- **[Whisper LCE](Whisper-LCE)** - M1noa's modified version
- **[Build Troubleshooting](Build-Troubleshooting)** - Common build issues
- **[Repository Overview](Repository-Overview)** - All editions

---

**Last Updated**: March 10, 2026  
**Maintained By**: M1noa
