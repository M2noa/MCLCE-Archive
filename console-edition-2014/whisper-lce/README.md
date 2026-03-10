# Whisper LCE - Modified by M1noa

> **⚠️ Important Notice**  
> This is **NOT** the original Whisper LCE version. This is **M1noa's modified fork** with significant enhancements, bug fixes, and improvements.

## About This Project

**Whisper LCE** is a modified version of the leaked Minecraft Legacy Console Edition (2014) source code. This fork has been extensively modified by **M1noa** to fix critical bugs, remove console limitations, and add PC-specific features.

### Key Features

- ✅ **3 Critical Bug Fixes** - Chest crashes, void spawns, random damage eliminated
- ✅ **Increased Limits** - TNT (20→100), Gravity blocks (20→200)
- ✅ **Infinite World Size** - 38× larger than original (32,768 blocks)
- ✅ **Sign Censoring Removed** - No profanity filtering
- ✅ **Enhanced Debug Menu** - F3 menu with command console
- ✅ **Chat System Re-enabled** - Full keyboard chat support
- ✅ **Professional Launcher** - Custom launcher with persistent settings

## Download This Project

```bash
git clone https://github.com/Minecraft-Community-Edition/client.git whisper-lce
cd whisper-lce
```

## Quick Start

### Requirements
- **Visual Studio 2022** (v143 toolset) or **Visual Studio 2012** (v110 toolset)
- Windows 10/11 x64
- Platform: **x64** only

### Building

1. Open `MinecraftConsoles.sln` in Visual Studio
2. Select **Release | x64** configuration
3. Build Solution (Ctrl+Shift+B)

**Output Files:**
- `x64\Release\Minecraft.Client.exe` - Main game
- `Launcher\x64\Release\Launcher.exe` - Launcher

### First Time Setup

If build fails with missing dependencies:
```batch
install_4jlibs.bat
```

Then rebuild the solution.

## Documentation

- **[Build Instructions](WIKI_Build_Instructions.md)** - Detailed build guide
- **[Changes from Original](WIKI_Changes_from_Original.md)** - Complete changelog
- **[Bug Fixes Summary](BUG_FIXES_SUMMARY.md)** - All bugs fixed
- **[TODO Roadmap](TODO.md)** - Future plans
- **[Build Troubleshooting](BUILD_TROUBLESHOOTING.md)** - Common issues

## What's Different from Original Console Edition?

This modified version includes:

### Critical Bug Fixes
1. **Chest Crash Fix** - Overstacked items (255 stacks) no longer crash the game
2. **Void Spawn Fix** - No more spawning in void when logging out on glass/slabs
3. **Random Damage Fix** - Eliminated random suffocation damage near walls

### Limit Increases
4. **TNT Limit** - Increased from 20 to 100 entities (5× increase)
5. **Gravity Blocks** - Increased from 20 to 200 blocks (10× increase)

### Features Added
6. **Infinite World Size** - 864 → 32,768 blocks (38× larger)
7. **Sign Censoring Removed** - Full creative freedom
8. **Enhanced Debug Menu** - F3 menu with command console
9. **Chat System** - Full keyboard support
10. **Professional Launcher** - Settings and configuration

See [WIKI_Changes_from_Original.md](WIKI_Changes_from_Original.md) for complete details.

## Project Structure

```
whisper-lce/
├── Minecraft.Client/        # Main game client code
├── Minecraft.World/         # World generation and management
├── Launcher/                # Custom launcher
├── Windows_Libs/            # 4JLibs dependencies
├── LCE-Cheat/              # Cheat/mod tools
└── .github/                # GitHub templates and workflows
```

## Build Configurations

- **v143** (VS 2022 / MSVC 14.3) - Recommended for modern systems
- **v110** (VS 2012) - Original toolset, guaranteed compatibility

If you encounter linker errors with v143, switch to v110 toolset in project properties.

## Credits & Attribution

**Original Source**: Minecraft Console Edition (2014) - 4J Studios / Mojang  
**Modified By**: M1noa  
**Repository**: https://github.com/Minecraft-Community-Edition/client

This project is based on the leaked Console Edition source code. All modifications, bug fixes, and enhancements are by M1noa.

## Statistics

| Metric | Value |
|--------|-------|
| **Files Modified** | 93 |
| **Files Created** | 18 |
| **Lines Changed** | ~1,600 |
| **Critical Bugs Fixed** | 3 |
| **Features Added** | 8 |
| **Documentation** | 18 files (~95KB) |

## Legal Notice

This is leaked source code from Minecraft Console Edition (2014). Use at your own risk. This project is for educational and preservation purposes only.

## Contributing

Contributions are welcome! Please read the contributing guidelines before submitting pull requests.

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Support

- **Issues**: Report bugs via [GitHub Issues](https://github.com/Minecraft-Community-Edition/client/issues)
- **Discussions**: Join the conversation in [GitHub Discussions](https://github.com/Minecraft-Community-Edition/client/discussions)
- **Wiki**: Check the [Wiki](https://github.com/Minecraft-Community-Edition/client/wiki) for detailed documentation

---

**Last Updated**: March 10, 2026  
**Version**: Pre-release (ready to build)  
**Status**: ✅ READY TO BUILD
