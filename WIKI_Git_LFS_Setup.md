# Git LFS Setup

Guide for setting up Git Large File Storage (LFS) for the MCLCE Archive.

## What is Git LFS?

Git LFS (Large File Storage) is an extension that replaces large files with text pointers inside Git, while storing the file contents on a remote server.

**Why We Use It**: Many files in this archive are >100MB (Story Mode archives, debug symbols, compiled binaries).

---

## Installation

### Windows

**Option 1: Chocolatey**
```bash
choco install git-lfs
```

**Option 2: Manual Download**
1. Download from https://git-lfs.github.com/
2. Run the installer
3. Open Git Bash or Command Prompt
4. Run: `git lfs install`

### macOS

**Using Homebrew**:
```bash
brew install git-lfs
git lfs install
```

### Linux

**Debian/Ubuntu**:
```bash
sudo apt-get install git-lfs
git lfs install
```

**Fedora**:
```bash
sudo dnf install git-lfs
git lfs install
```

**Arch Linux**:
```bash
sudo pacman -S git-lfs
git lfs install
```

---

## Verification

Check if Git LFS is installed:

```bash
git lfs version
```

Expected output:
```
git-lfs/3.x.x (GitHub; windows amd64; go 1.x.x)
```

---

## Usage

### For New Clones

Clone with LFS support:
```bash
git clone https://github.com/M2noa/MCLCE-Archive.git
cd MCLCE-Archive
```

Git LFS will automatically download large files.

### For Existing Clones

If you cloned before installing LFS:

```bash
cd MCLCE-Archive
git lfs install
git lfs pull
```

---

## LFS Files in This Repository

| Extension | Description | Count | Total Size |
|-----------|-------------|-------|------------|
| `*.ttarch2` | Story Mode game archives | ~50 | ~13 GB |
| `*.pdb` | Debug symbol files | ~100 | ~2 GB |
| `*.ilk` | Incremental linker files | ~50 | ~500 MB |
| `*.pak` | Package files | ~20 | ~300 MB |
| `*.bank` | Audio bank files | ~30 | ~200 MB |

**Total LFS Files**: ~250  
**Total LFS Size**: ~16 GB

---

## Commands

### List LFS Files

```bash
git lfs ls-files
```

### Check LFS Status

```bash
git lfs status
```

### Pull LFS Files

```bash
git lfs pull
```

### Fetch LFS Files (without checkout)

```bash
git lfs fetch
```

### Prune Old LFS Files

```bash
git lfs prune
```

---

## Bandwidth Limits

GitHub has LFS bandwidth limits:

- **Free accounts**: 1 GB/month
- **Pro accounts**: 50 GB/month

### If You Hit the Limit

1. **Wait for reset** - Limits reset monthly
2. **Use sparse checkout** - Download only needed editions
3. **Skip LFS files** - Clone without LFS (see below)
4. **Use archive mirrors** - Download from alternative sources

---

## Cloning Without LFS

If you don't need large binary files:

```bash
# Skip LFS files during clone
GIT_LFS_SKIP_SMUDGE=1 git clone https://github.com/M2noa/MCLCE-Archive.git
```

This will clone text pointers instead of actual files, saving bandwidth.

---

## Troubleshooting

### Error: "git-lfs filter not found"

**Solution**: Install Git LFS and run `git lfs install`

### Error: "batch response: This repository is over its data quota"

**Solution**: 
- Wait for bandwidth reset
- Use browser download instead
- Download from archive mirrors

### LFS Files Show as Text Pointers

**Solution**: Pull LFS files:
```bash
git lfs pull
```

### Slow LFS Downloads

**Solution**:
- Use sparse checkout for specific editions
- Download during off-peak hours
- Use archive mirrors

---

## Configuration

### Global LFS Settings

```bash
# Set LFS cache directory
git config --global lfs.storage /path/to/lfs/cache

# Set concurrent transfers
git config --global lfs.concurrenttransfers 8
```

### Repository-Specific Settings

```bash
# Skip LFS for this repo
git config lfs.fetchexclude "*"

# Only fetch specific file types
git config lfs.fetchinclude "*.pdb,*.ttarch2"
```

---

## Alternative Download Methods

If Git LFS doesn't work:

1. **Browser Download**: Use download-directory.github.io
2. **Archive Mirrors**: See [Repository Overview](Repository-Overview) for mirror links
3. **Torrent** (if available): Check archive mirrors

---

## Learn More

- **Official Docs**: https://git-lfs.github.com/
- **GitHub LFS Guide**: https://docs.github.com/en/repositories/working-with-files/managing-large-files
- **Download Guide**: [Download Guide](Download-Guide)

---

**Last Updated**: March 10, 2026  
**Maintained By**: M1noa
