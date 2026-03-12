<p align="center">
  <h1 align="center">MCLCE Archive</h1>
  <p align="center">Comprehensive archive of Minecraft Legacy Console Edition and related source code leaks from 2013-2021</p>
</p>

## Download This Repository

### Quick Clone (without large files)

```bash
git clone https://github.com/M2noa/MCLCE-Archive.git
```

### Download Large Files (LFS)

This repository uses a custom LFS server at `litter.minoa.cat` for large files (archives, binaries, etc).

**Setup:**

```bash
# Clone the repository
git clone https://github.com/M2noa/MCLCE-Archive.git
cd MCLCE-Archive

# Install Git LFS
# macOS: brew install git-lfs
# Linux: sudo apt-get install git-lfs
# Windows: Download from https://git-lfs.com

# Configure custom LFS server
git config -f .lfsconfig lfs.url https://litter.minoa.cat/lfs

# Download all LFS files
git lfs pull
```

**Future pulls:**

```bash
git pull
git lfs pull
```

[Download via Browser](https://download-directory.github.io/?url=https://github.com/M2noa/MCLCE-Archive) (Note: Does not include LFS files)

> [!IMPORTANT]
> GitHub's LFS quota was exceeded. Large files are now hosted on a custom server at litter.minoa.cat.

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

Large files are stored using Git LFS on a custom server:

| Extension | Description |
|-----------|-------------|
| `*.ttarch2` | Story Mode game archives |
| `*.pdb` | Debug symbol files |
| `*.ilk` | Incremental linker files |
| `*.pak` | Package files |
| `*.bank` | Audio bank files |
| `*.zip`, `*.7z`, `*.rar` | Archive files |
| `*.lib` | Library files |

### Why Custom LFS Server?

GitHub's LFS storage quota was exceeded. All LFS files are now hosted on `litter.minoa.cat` with:
- Automatic deduplication via SHA-256
- Chunked uploads for files >50MB
- Up to 2GB per file

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

## Disclaimer

> [!IMPORTANT]
> This repository contains archived leaked source code for preservation and educational purposes only. If you are Microsoft Corporation, Mojang AB, 4J Studios, or any other rights holder and believe this repository contains copyrighted material that infringes on your rights, please contact the repository maintainer to discuss removal or resolution.
>
> **This repository is not affiliated with, endorsed by, or supported by Microsoft, Mojang, or 4J Studios.**

---

<p align="center">
  <strong>Note:</strong> This is an archival repository for preservation and educational purposes.
</p>
