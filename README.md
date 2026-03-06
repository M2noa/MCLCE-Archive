# MCLCE Archive

Comprehensive archive of Minecraft Legacy Console Edition and related source code leaks from 2013-2021.

## Repository Contents

This repository contains organized source code from multiple Minecraft editions:

- **Console Edition (2014)** - PS3, PS4, PS Vita, Xbox 360, Xbox One, Windows x86_64
- **Pocket Edition (2013)** - Android, iOS, Raspberry Pi (MCPE 0.6.1)
- **Infdev (2014)** - PC custom game engine
- **Dungeons & Legends** - Spinoff game builds and source code
- **Story Mode** - Steam beta branch builds
- **Pre-compiled Windows** - Ready-to-run Windows build

## Structure

```
MCLCE-Archive/
├── console-edition-2014/       # Console Edition source code
│   ├── full-multiplatform/     # All platforms (PS3, PS4, Vita, Xbox 360, Xbox One, Windows)
│   ├── oct-2014-backup/        # October 2014 snapshot
│   ├── ps3-only-stripped/      # Initial PS3-only leak
│   └── dec-2014-build/         # December 2014 build
├── pocket-edition-2013/        # MCPE 0.6.1 C++ source
├── infdev-2014/                # PC Infdev custom engine
├── dungeons-legends/           # Dungeons & Legends builds
│   ├── dungeons-source/        # Dungeons C++ source code
│   ├── dungeons-original/      # Original Dungeons build
│   ├── legends-original/       # Original Legends build (MCBE 1.2.9.0)
│   └── narration-build-2017/   # Dungeons dev build (MCBE 0.15.0.0)
├── story-mode/                 # Story Mode beta branches
│   ├── enderman/               # Steam beta branch
│   ├── endercon/               # Steam beta branch
│   ├── linux-test/             # Steam beta branch
│   └── patch/                  # Steam beta branch
└── precompiled-windows/        # Windows x86_64 executable + assets
```

## Documentation

- **[ARCHIVES.md](ARCHIVES.md)** - Detailed overview of all editions, sizes, and technical specifications
- **[SOURCES.md](SOURCES.md)** - Complete source mapping, download URLs, git hashes, and technical notes
- **Edition READMEs** - Each folder contains detailed information about that specific edition

## Leak Timeline

1. **2014** - PS3-only source code (stripped, modified timestamps)
2. **2014** - Full console edition multi-platform source
3. **2014** - MCPE (Pocket Edition) 0.6.1 source code
4. **2017-2021** - Dungeons & Legends builds
5. **2024+** - Story Mode beta branches

## Technical Notes

### Console Edition
- Java-like build on C++ (not Bedrock-based)
- Multi-platform support with platform-specific implementations
- Complete rendering, networking, and game logic

### Pocket Edition
- Version 0.6.1 C++ source code
- Cross-platform mobile implementation

### Dungeons & Legends
- **Dungeons Narration**: Based on MCBE 0.15.0.0, can switch between Dungeons and regular MCBE mode (Ctrl+E)
- **Legends**: Based on MCBE 1.2.9.0 Win32 Dev with .PDB debug symbols

### Story Mode
- All builds modified to run without Steam
- Cracked using GitHub tool to remove Steam DRM

## Git LFS

Large files (>100MB) are stored using Git LFS:
- `*.ttarch2` - Story Mode game archives
- `*.pdb` - Debug symbol files
- `*.ilk` - Incremental linker files
- `*.pak` - Package files
- `*.bank` - Audio bank files

## Original Sources

- Console Edition: https://bin.skylvi.net/Leaks/Minecraft/
- Dungeons source: https://drive.proton.me/urls/EVM0GRC0MM#WckH5oSa6fOn
- Dungeons/Legends builds: https://hazbinhelluvageneral.com/cubevideojuego/cubegame
- Legends: https://archive.org/details/minecraft-console-legacy-edition-source-code-leak
- Story Mode: https://hazbinhelluvageneral.com/cubevideojuego/cubegame

## DMCA Information

For information on DMCA takedown requests: https://bin.skylvi.net/dmca

## Archive Information

- **Total Size:** ~35.5 GB
- **Files:** 80,000+ files
- **Commits:** 13 commits documenting extraction and organization
- **Last Updated:** 2026-03-05

---

**Note:** This is an archival repository for preservation and educational purposes. All original sources and leak information are documented in SOURCES.md.
