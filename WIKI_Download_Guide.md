# Download Guide

How to download specific editions from the MCLCE Archive.

## Full Repository

Download everything:

```bash
git clone https://github.com/M2noa/MCLCE-Archive.git
cd MCLCE-Archive
```

**Size**: ~20 GB (with Git LFS)  
**Time**: 30-60 minutes depending on connection

---

## Sparse Checkout (Recommended)

Download only what you need using sparse checkout.

### Console Edition Only

```bash
git clone --depth 1 --filter=blob:none --sparse https://github.com/M2noa/MCLCE-Archive.git
cd MCLCE-Archive
git sparse-checkout set console-edition-2014
```

### Whisper LCE Only

```bash
git clone --depth 1 --filter=blob:none --sparse https://github.com/M2noa/MCLCE-Archive.git
cd MCLCE-Archive
git sparse-checkout set console-edition-2014/whisper-lce
```

### Pocket Edition Only

```bash
git clone --depth 1 --filter=blob:none --sparse https://github.com/M2noa/MCLCE-Archive.git
cd MCLCE-Archive
git sparse-checkout set pocket-edition-2013
```

### Dungeons & Legends Only

```bash
git clone --depth 1 --filter=blob:none --sparse https://github.com/M2noa/MCLCE-Archive.git
cd MCLCE-Archive
git sparse-checkout set dungeons-legends
```

### Story Mode Only

```bash
git clone --depth 1 --filter=blob:none --sparse https://github.com/M2noa/MCLCE-Archive.git
cd MCLCE-Archive
git sparse-checkout set story-mode
```

---

## Browser Download

Use download-directory.github.io for browser-based downloads:

### Full Repository
[Download Full Archive](https://download-directory.github.io/?url=https://github.com/M2noa/MCLCE-Archive)

### Console Edition
[Download Console Edition](https://download-directory.github.io/?url=https://github.com/M2noa/MCLCE-Archive/tree/main/console-edition-2014)

### Whisper LCE
[Download Whisper LCE](https://download-directory.github.io/?url=https://github.com/M2noa/MCLCE-Archive/tree/main/console-edition-2014/whisper-lce)

### Pocket Edition
[Download Pocket Edition](https://download-directory.github.io/?url=https://github.com/M2noa/MCLCE-Archive/tree/main/pocket-edition-2013)

### Dungeons & Legends
[Download Dungeons & Legends](https://download-directory.github.io/?url=https://github.com/M2noa/MCLCE-Archive/tree/main/dungeons-legends)

### Story Mode
[Download Story Mode](https://download-directory.github.io/?url=https://github.com/M2noa/MCLCE-Archive/tree/main/story-mode)

---

## Git LFS Setup

Large files (>100MB) require Git LFS.

### Install Git LFS

**Windows**:
```bash
# Using Chocolatey
choco install git-lfs

# Or download from: https://git-lfs.github.com/
```

**macOS**:
```bash
brew install git-lfs
```

**Linux**:
```bash
# Debian/Ubuntu
sudo apt-get install git-lfs

# Fedora
sudo dnf install git-lfs

# Arch
sudo pacman -S git-lfs
```

### Initialize Git LFS

After installing:
```bash
git lfs install
```

### Pull LFS Files

If you cloned without LFS:
```bash
git lfs pull
```

**Learn More**: [Git LFS Setup](Git-LFS-Setup)

---

## Download Sizes

| Edition | Without LFS | With LFS | Files |
|---------|-------------|----------|-------|
| Console Edition | ~500 MB | ~3 GB | 19,433+ |
| Pocket Edition | 63 MB | 63 MB | 2,100 |
| Dungeons & Legends | ~200 MB | ~1.5 GB | 17,401+ |
| Story Mode | ~2 GB | ~15 GB | 917 |
| Infdev | 4.6 MB | 4.6 MB | 430 |
| **Total** | ~3 GB | ~20 GB | 39,381+ |

---

## Troubleshooting

### Clone is Very Slow

Use sparse checkout to download only what you need:
```bash
git clone --depth 1 --filter=blob:none --sparse <url>
```

### Out of Disk Space

- Use sparse checkout for specific editions
- Skip Git LFS files if you don't need large binaries
- Download via browser and extract only needed files

### Git LFS Bandwidth Limit

GitHub has LFS bandwidth limits. If exceeded:
- Wait for limit reset (monthly)
- Use browser download instead
- Download from archive mirrors (see [Repository Overview](Repository-Overview))

### Clone Fails Midway

Resume the clone:
```bash
cd MCLCE-Archive
git fetch --all
git lfs pull
```

---

## Verification

After downloading, verify integrity:

```bash
# Check file count
find . -type f | wc -l

# Check repository status
git status

# Check LFS files
git lfs ls-files
```

---

**Last Updated**: March 10, 2026  
**Maintained By**: M1noa
