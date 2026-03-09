<p align="center">
  <h1 align="center">MCLCE Archive</h1>
  <p align="center">Comprehensive archive of Minecraft Legacy Console Edition and related source code leaks from 2013-2021</p>
</p>

## Download This Repository

```bash
git clone https://github.com/M2noa/MCLCE-Archive.git
```

[Download via Browser](https://download-directory.github.io/?url=https://github.com/M2noa/MCLCE-Archive)

> [!IMPORTANT]
> This isn't a permanent solution. I'm working on a custom archive. I wanted to dip my toes into the water by seeing how long it would take to get taken down from GitHub. I expect this to be taken down any day now - I'm surprised it hasn't lol...

---

## Repository Contents

This repository contains organized source code from multiple Minecraft editions:

| Edition | Platforms | Year | Size | Files |
|---------|-----------|------|------|-------|
| **Console Edition** | PS3, PS4, PS Vita, Xbox 360, Xbox One, Windows x86_64 | 2014 | ~3 GB | 19,433+ |
| **Pocket Edition** | Android, iOS, Raspberry Pi, Windows x86 | 2013 | 63 MB | 2,100 |
| **Infdev** | PC | 2014 | 4.6 MB | 430 |
| **Dungeons & Legends** | Multi-platform | 2017-2021 | ~1.5 GB | 17,401+ |
| **Story Mode** | Multi-platform (Steam beta) | 2024? | ~15 GB | 917 |
| **Pre-compiled Windows** | Windows x86_64 | 2014 | 810 MB | - |

---

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
├── precompiled-windows/        # Windows x86_64 executable + assets
└── archives/                   # Original downloaded archives
    ├── README.md               # Archive documentation and hashes
    └── download.bat            # Download script for all archives
```

---

## Technical Notes

### Console Edition
> [!NOTE]
> Java-like build system on C++ (not Bedrock)

- Multi-platform support with platform-specific implementations
- Complete rendering, networking, and game logic

### Pocket Edition
- Version 0.6.1 C++
- Cross-platform mobile implementation

### Dungeons & Legends
| Build | Base Version | Notes |
|-------|--------------|-------|
| Dungeons Narration | MCBE 0.15.0.0 | Can switch between Dungeons and regular MCBE mode (Ctrl+E) |
| Legends | MCBE 1.2.9.0 Win32 Dev | Includes .PDB debug symbols |

### Story Mode
> [!WARNING]
> All builds modified to run without Steam (Steam-cracked)

- Beta branches: enderman, endercon, linux-test, patch
- Branch passwords documented in `info_storymode.txt`

---

## Git LFS

Large files (>100MB) are stored using Git LFS:

| Extension | Description |
|-----------|-------------|
| `*.ttarch2` | Story Mode game archives |
| `*.pdb` | Debug symbol files |
| `*.ilk` | Incremental linker files |
| `*.pak` | Package files |
| `*.bank` | Audio bank files |

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

<p align="center">
  <strong>Note:</strong> This is an archival repository for preservation and educational purposes.
</p>
