# Minecraft Console Edition (2014)

> [!NOTE]
> Source code for Minecraft Console Edition from 2014 leaks. Java-like build system on C++ (not Bedrock-based).

---

## Overview

| Attribute | Value |
|-----------|-------|
| **Platforms** | PS3, PS4 (Orbis), PS Vita, Xbox 360, Xbox One (Durango), Windows x86_64 |
| **Total Size** | ~3 GB |
| **Files** | 19,433+ |
| **Language** | C++ |
| **Year** | 2014 |

---

## Folders

### full-multiplatform/

| Attribute | Value |
|-----------|-------|
| **Source** | `Minecraft.zip` (gofile.io/d/RRf1hv) |
| **Date** | 2014 |
| **Platforms** | PS3, PS4 (Orbis), PS Vita, Xbox 360, Xbox One (Durango), Windows x86_64 |

Complete multi-platform console edition source code. Contains platform-specific folders for all supported consoles.

**Platform Structure:**
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

| Attribute | Value |
|-----------|-------|
| **Source** | `mc-console-oct2014.zip` (gofile.io/d/pxeEVD) |
| **Date** | October 10, 2014 |
| **Git Hash** | `5ca1eff0124dd98f5bab76cfd2052dc8718b1c18` |

Backup snapshot from October 2014. Similar structure to full-multiplatform.

---

### ps3-only-stripped/

| Attribute | Value |
|-----------|-------|
| **Source** | `Minecraft PS3 Edition Source Code.zip` (gofile.io/d/NIawxk) |
| **Date** | 2014 (modified timestamps) |
| **Platforms** | PS3 only |

> [!WARNING]
> Initial leak - PS3 edition with other platforms stripped out and file timestamps modified.

This was the first leak before the full multi-platform version was released.

---

### dec-2014-build/

| Attribute | Value |
|-----------|-------|
| **Source** | `MinecraftConsoles-141217.zip` (gofile.io/d/pxeEVD) |
| **Date** | December 17, 2014 |

Console edition build from December 2014. Contains Visual Studio solution file.

---

## Leak History

```
1st Leak ──► PS3-only (stripped, modified timestamps)
             └─► "Minecraft PS3 edition source code leak"

2nd Leak ──► Full multi-platform source
             └─► Actual complete console edition (2014)

3rd Leak ──► MCPE source code
             └─► Pocket Edition 0.6.1 (see ../pocket-edition-2013/)
```

---

## Build Requirements

> [!IMPORTANT]
> Building requires Visual Studio 2012 and various proprietary libraries (Iggy, Miles, Sentient, 4JLibs).

See `lcemp/README.md` for detailed build instructions.

**Launch Arguments:**
- `-name <playername>` - Set player name
- `-ip <address>` - Set server IP
- `-port <port>` - Set server port

---

## Sources

| Source | URL |
|--------|-----|
| Original Thread | https://boards.4chan.org/g/thread/108264778/minecraft-ps3-edition-source-code |
| DMCA Info | https://bin.skylvi.net/dmca |
