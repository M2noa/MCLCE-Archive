# Credits & Attribution

This page documents the sources, contributors, and attributions for Whisper LCE.

## Project Information

**Project Name**: Whisper LCE (Modified)  
**Maintainer**: M1noa  
**Repository**: https://github.com/Minecraft-Community-Edition/client  
**License**: Educational/Preservation (Leaked Source Code)

---

## Original Source Code

### Minecraft Console Edition (2014)

**Developer**: 4J Studios  
**Publisher**: Mojang AB / Microsoft  
**Platform**: PS3, PS4, PS Vita, Xbox 360, Xbox One, Windows  
**Version**: TU14 era (2014)  
**Status**: Leaked source code

**Original Leak Sources**:
- `Minecraft.zip` (gofile.io/d/RRf1hv)
- Full multi-platform source code
- Leaked in 2014

**Original Thread**: https://boards.4chan.org/g/thread/108264778/minecraft-ps3-edition-source-code

---

## Modifications & Enhancements

### Primary Developer

**M1noa** (M2noa)
- All bug fixes and enhancements
- Build system configuration
- Documentation and wiki
- Project maintenance

**Contributions**:
- Fixed 3 critical bugs (chest crash, void spawn, random damage)
- Increased entity limits (TNT, gravity blocks)
- Added 8 new features (debug menu, infinite worlds, etc.)
- Created 18 documentation files (~95KB)
- Modified ~1,600 lines of code across 93 files

---

## Community Contributions

### Pull Requests

**rodrigothe1st**
- Fixed typo in documentation
- PR: https://github.com/Minecraft-Community-Edition/client/pull/2

### Issue Reports

Contributors who reported bugs and issues:
- (List will be updated as issues are filed)

---

## Third-Party Libraries

### 4JLibs (4J Studios Proprietary Libraries)

**Developer**: 4J Studios  
**License**: Proprietary (included in leak)  
**Components**:
- Input.lib - Input handling
- Storage.lib - File I/O and storage
- Render.lib - Rendering engine
- Profile.lib - User profiles

**Source**: https://github.com/4J-Studios (if available)

### ImGui (Dear ImGui)

**Developer**: Omar Cornut (ocornut)  
**License**: MIT License  
**Purpose**: Debug menu UI (LCE-Cheat component)  
**Repository**: https://github.com/ocornut/imgui

**Used In**: LCE-Cheat overlay and debug tools

### Visual Studio

**Developer**: Microsoft Corporation  
**Toolset**: v143 (Visual Studio 2022) / v110 (Visual Studio 2012)  
**License**: Proprietary

---

## Documentation Sources

### Build Instructions

Based on:
- Original Console Edition build system
- Community reverse-engineering efforts
- Trial and error by M1noa

### Bug Analysis

Research and analysis by:
- M1noa (primary investigator)
- Community bug reports
- Code inspection and debugging

---

## Archive & Preservation

### MCLCE Archive

**Maintainer**: M2noa (M1noa)  
**Repository**: https://github.com/M2noa/MCLCE-Archive  
**Purpose**: Comprehensive archive of Minecraft leaks

**Contents**:
- Console Edition (2014)
- Pocket Edition (2013)
- Dungeons & Legends (2017-2021)
- Story Mode (2024)
- Infdev (2014)

### Original Archive Mirrors

**Sources**:
- https://bin.skylvi.net/Leaks/Minecraft/
- https://hazbinhelluvageneral.com/cubevideojuego/
- https://archive.org/details/minecraft-console-legacy-edition-source-code-leak

---

## Special Thanks

### Community

- **Minecraft Community Edition Team** - For hosting and supporting the project
- **4chan /g/ board** - Original leak discovery and discussion
- **Minecraft modding community** - Inspiration and technical knowledge
- **Legacy Console Edition players** - Keeping the community alive

### Tools & Resources

- **Visual Studio** - Development environment
- **Git** - Version control
- **GitHub** - Hosting and collaboration
- **Markdown** - Documentation format

---

## Legal Notice

### Copyright

**Minecraft** is a trademark of Mojang AB / Microsoft Corporation.

**Console Edition** was developed by 4J Studios under license from Mojang.

This project uses **leaked source code** from 2014. All original code is copyright of 4J Studios and Mojang AB.

### Modifications

All modifications, bug fixes, and enhancements made by M1noa are provided as-is for educational and preservation purposes.

### Disclaimer

This project is **not affiliated with, endorsed by, or supported by**:
- Mojang AB
- Microsoft Corporation
- 4J Studios
- Xbox Game Studios

**Use at your own risk.**

---

## Attribution Requirements

If you use or distribute this modified version, please include:

1. **Original Source Attribution**:
   - "Based on Minecraft Console Edition (2014) by 4J Studios / Mojang"

2. **Modification Attribution**:
   - "Modified by M1noa"
   - Link to repository: https://github.com/Minecraft-Community-Edition/client

3. **Disclaimer**:
   - "This is leaked source code. Use at your own risk."

---

## Code Attribution

### Files with Significant Modifications

| File | Original Author | Modified By | Changes |
|------|----------------|-------------|---------|
| `Minecraft.World\ItemInstance.cpp` | 4J Studios | M1noa | Chest crash fix |
| `Minecraft.Client\PlayerList.cpp` | 4J Studios | M1noa | Void spawn fix |
| `Minecraft.World\Entity.cpp` | 4J Studios | M1noa | Random damage fix |
| `Minecraft.Client\ServerLevel.h` | 4J Studios | M1noa | Entity limit increases |
| `Launcher/Launcher.cpp` | M1noa | M1noa | New file (launcher) |

### New Files Created

All files in the following directories were created by M1noa:
- `Launcher/` - Custom launcher
- `*.md` (documentation files) - All documentation
- `.github/` - GitHub templates and workflows

---

## Version History

### Pre-release (March 10, 2026)
- Initial modified version by M1noa
- 3 critical bugs fixed
- 8 features added
- Build system configured
- Documentation complete

### Original (2014)
- Leaked Console Edition source code
- TU14 era
- Multi-platform support

---

## Contact

### Project Maintainer

**M1noa** (M2noa)
- GitHub: https://github.com/M2noa
- Repository: https://github.com/Minecraft-Community-Edition/client

### Reporting Issues

- **Bug Reports**: https://github.com/Minecraft-Community-Edition/client/issues
- **Feature Requests**: https://github.com/Minecraft-Community-Edition/client/issues
- **Discussions**: https://github.com/Minecraft-Community-Edition/client/discussions

---

## Acknowledgments

Special thanks to everyone who contributed to preserving and improving Minecraft Legacy Console Edition.

This project exists to preserve gaming history and provide educational value to the community.

---

**Last Updated**: March 10, 2026  
**Maintained By**: M1noa  
**Status**: Active Development
