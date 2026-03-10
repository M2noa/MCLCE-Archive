# Repository Overview

Complete overview of all editions and content in the MCLCE Archive.

## Repository Structure

```
MCLCE-Archive/
├── console-edition-2014/       # Console Edition source code
│   ├── full-multiplatform/     # All platforms
│   ├── oct-2014-backup/        # October 2014 snapshot
│   ├── ps3-only-stripped/      # Initial PS3-only leak
│   ├── dec-2014-build/         # December 2014 build
│   └── whisper-lce/            # M1noa's modified version ⭐
├── pocket-edition-2013/        # MCPE 0.6.1 C++ source
├── infdev-2014/                # PC Infdev custom engine
├── dungeons-legends/           # Dungeons & Legends builds
│   ├── dungeons-source/        # Dungeons C++ source code
│   ├── dungeons-original/      # Original Dungeons build
│   ├── legends-original/       # Original Legends build
│   └── narration-build-2017/   # Dungeons dev build
├── story-mode/                 # Story Mode beta branches
│   ├── enderman/               # Steam beta branch
│   ├── endercon/               # Steam beta branch
│   ├── linux-test/             # Steam beta branch
│   └── patch/                  # Steam beta branch
├── precompiled-windows/        # Windows x86_64 executable
└── archives/                   # Original downloaded archives
```

---

## Editions Overview

### Console Edition (2014)

**Platforms**: PS3, PS4, PS Vita, Xbox 360, Xbox One, Windows x86_64  
**Size**: ~3 GB  
**Files**: 19,433+  
**Language**: C++

Complete multi-platform console edition source code. Java-like build system on C++ (not Bedrock-based).

**Variants**:
- `full-multiplatform/` - Complete source for all platforms
- `oct-2014-backup/` - October 10, 2014 snapshot
- `ps3-only-stripped/` - Initial PS3-only leak (modified timestamps)
- `dec-2014-build/` - December 17, 2014 build
- `whisper-lce/` - **M1noa's modified version** ⭐

**Learn More**: [Console Edition](Console-Edition) | [Whisper LCE](Whisper-LCE)

---

### Pocket Edition (2013)

**Platforms**: Android, iOS, Raspberry Pi, Windows x86  
**Size**: 63 MB  
**Files**: 2,100  
**Version**: 0.6.1  
**Language**: C++

Cross-platform mobile implementation from 2013.

**Learn More**: [Pocket Edition](Pocket-Edition)

---

### Dungeons & Legends (2017-2021)

**Platforms**: Multi-platform  
**Size**: ~1.5 GB  
**Files**: 17,401+  
**Language**: C++ (Bedrock-based)

| Build | Base Version | Notes |
|-------|--------------|-------|
| Dungeons Narration | MCBE 0.15.0.0 | Can switch between Dungeons and MCBE mode (Ctrl+E) |
| Legends | MCBE 1.2.9.0 Win32 Dev | Includes .PDB debug symbols |

**Learn More**: [Dungeons & Legends](Dungeons-and-Legends)

---

### Story Mode (2024)

**Platforms**: Multi-platform (Steam beta)  
**Size**: ~15 GB  
**Files**: 917  
**Branches**: enderman, endercon, linux-test, patch

> **⚠️ Warning**: All builds modified to run without Steam (Steam-cracked)

**Learn More**: [Story Mode](Story-Mode)

---

### Infdev (2014)

**Platform**: PC  
**Size**: 4.6 MB  
**Files**: 430  
**Year**: 2014

PC Infdev custom engine.

**Learn More**: [Infdev](Infdev)

---

### Precompiled Windows (2014)

**Platform**: Windows x86_64  
**Size**: 810 MB  
**Type**: Executable + assets

Pre-compiled Windows build with assets.

---

## Leak History

```
Timeline:

2013 ──► Pocket Edition 0.6.1 leak

2014 ──► Console Edition leaks:
         ├─► PS3-only (stripped, modified timestamps)
         ├─► Full multi-platform source
         ├─► October 2014 backup
         └─► December 2014 build

2017-2021 ──► Dungeons & Legends builds

2024 ──► Story Mode Steam beta branches

2026 ──► Whisper LCE (M1noa's modifications) ⭐
```

---

## Original Sources

| Source | URL |
|--------|-----|
| Console Edition | https://bin.skylvi.net/Leaks/Minecraft/ |
| Dungeons source | https://drive.proton.me/urls/EVM0GRC0MM#WckH5oSa6fOn |
| Dungeons/Legends builds | https://hazbinhelluvageneral.com/cubevideojuego/ |
| Legends | https://archive.org/details/minecraft-console-legacy-edition-source-code-leak |
| Story Mode | https://hazbinhelluvageneral.com/cubevideojuego/ |
| Archive Mirror | https://hazbinhelluvageneral.com/cubevideojuego/cubegame |

---

## Git LFS Files

Large files (>100MB) are stored using Git LFS:

| Extension | Description |
|-----------|-------------|
| `*.ttarch2` | Story Mode game archives |
| `*.pdb` | Debug symbol files |
| `*.ilk` | Incremental linker files |
| `*.pak` | Package files |
| `*.bank` | Audio bank files |

**Setup**: See [Git LFS Setup](Git-LFS-Setup)

---

## Legal Notice

This is an archival repository containing leaked source code from various Minecraft editions. All original code is copyright of their respective owners (Mojang AB, Microsoft Corporation, 4J Studios, Telltale Games).

**Use at your own risk. For educational and preservation purposes only.**

---

**Last Updated**: March 10, 2026  
**Maintained By**: M1noa
