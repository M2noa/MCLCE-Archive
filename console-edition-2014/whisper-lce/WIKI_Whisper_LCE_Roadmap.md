# Roadmap

This page outlines the future development plans for Whisper LCE.

## Current Status

**Version**: Pre-release (ready to build)  
**Last Updated**: March 10, 2026  
**Maintainer**: M1noa

### Completed ✅
- 3 Critical bug fixes (chest crash, void spawn, random damage)
- 2 Limit increases (TNT, gravity blocks)
- 8 Features added (debug menu, infinite worlds, etc.)
- Build system configured
- Documentation complete

### In Progress 🔄
- Performance profiling and optimization
- Testing compiled builds

---

## Phase 1: Stability & Polish (Current)

**Goal**: Make the game stable and playable on PC

### Critical Fixes
- [x] Fix chest crash bug
- [x] Fix void spawn bug
- [x] Fix random damage bug
- [ ] Fix performance freezes (needs profiling)
- [ ] Fix lag blocks (network latency - may not be fixable)

### PC-Specific Features
- [x] Enhanced debug menu (F3)
- [x] Chat system re-enabled
- [x] Professional launcher
- [ ] Full mouse navigation for all menus
- [ ] Mouse sensitivity & raw input
- [ ] Rebindable keyboard controls

### Build System
- [x] 4JLibs integration
- [x] Visual Studio 2022 support
- [x] Build documentation
- [ ] Automated testing
- [ ] CI/CD pipeline (GitHub Actions)

---

## Phase 2: Remove Console Limitations

**Goal**: Uncap artificial console limits and fix console-specific bugs

### World Generation
- [x] Infinite world size (38× larger)
- [ ] Remove invisible bedrock walls completely
- [ ] Implement true infinite generation (or Java's 30M limit)
- [ ] Fix Nether Wart spawning (should only be in Fortresses)
- [ ] Fix Wither Skeletons spawning everywhere

### Entity & Mob Limits
- [x] TNT limit increased (20→100)
- [x] Gravity blocks increased (20→200)
- [ ] Remove item frame/painting limits
- [ ] Remove minecart limits
- [ ] Replace hardcoded mob caps with Java's chunk-based system

### Console-Specific Bugs
- [ ] Fix void behavior in Creative (should kill, not bounce)
- [ ] Fix Wither AI (remove constant dashing)
- [ ] Move auto-save to background thread (remove 15-min freeze)

---

## Phase 3: Content Updates (1.7-1.8)

**Goal**: Bring Console Edition up to Java 1.8 feature parity

### 1.7 "Update That Changed The World"

#### World Generation
- [ ] Implement 1.7 Temperature/Humidity biome system
- [ ] Add new biomes:
  - [ ] Mesa/Badlands
  - [ ] Savanna
  - [ ] Roofed Forest (Dark Forest)
  - [ ] Birch Forest
  - [ ] Mega Taiga
  - [ ] Flower Forest
  - [ ] Deep Ocean
  - [ ] Ice Spikes
- [ ] Add AMPLIFIED world type
- [ ] Prevent desert/snow biome adjacency

#### Blocks & Items
- [ ] Stained Glass and Stained Glass Panes
- [ ] Acacia and Dark Oak wood sets
- [ ] Packed Ice, Podzol, Red Sand
- [ ] New flowers (Allium, Azure Bluet, Blue Orchid, etc.)
- [ ] 2-tall plants (Peony, Rose Bush, Lilac, Sunflower)

#### Fishing Overhaul
- [ ] Add Salmon, Pufferfish, Clownfish
- [ ] Add fishing rod enchantments (Lure, Luck of the Sea)
- [ ] Add Water Breathing potions

#### Commands
- [ ] `/summon`
- [ ] `/setblock`
- [ ] `/tellraw`
- [ ] `/testforblock`
- [ ] Minecart with Command Block

### 1.8 "Bountiful Update"

#### Engine Changes
- [ ] Refactor metadata (0-15) to Block States system
- [ ] Implement Slime Block physics
- [ ] Add Advanced Cave Culling

#### Blocks & Items
- [ ] Granite, Diorite, Andesite (+ polished)
- [ ] Prismarine, Prismarine Bricks, Dark Prismarine
- [ ] Sea Lanterns
- [ ] Wet Sponges (absorb water)
- [ ] Slime Blocks and Iron Trapdoors
- [ ] Coarse Dirt
- [ ] Banners (6-layer NBT patterns)
- [ ] Armor Stands (NBT posing)
- [ ] All wood variants of Doors, Fences, Fence Gates
- [ ] Mutton, Rabbit (meat/stew/hide/foot)

#### Structures & Mobs
- [ ] Ocean Monuments
- [ ] Guardians and Elder Guardians
- [ ] Rabbits (8 skins including Killer Bunny)
- [ ] Endermites

#### Gameplay Mechanics
- [ ] Enchanting overhaul (Lapis Lazuli requirement)
- [ ] Villager career system
- [ ] Villager trade unlocking
- [ ] Farmer villagers harvest/replant crops
- [ ] Spectator Mode (Gamemode 3)
- [ ] Depth Strider enchantment

---

## Phase 4: Modern Features (Optional)

**Goal**: Backport popular modern features while preserving 1.8 feel

### Optional Backports
- [ ] Elytra flight (1.9)
- [ ] Shulker boxes (1.11)
- [ ] Concrete blocks (1.12)
- [ ] Tridents and underwater combat (1.13)
- [ ] Toggle in settings to enable/disable modern features

---

## Phase 5: Online & Multiplayer

**Goal**: Improve online experience and add crossplay

### Online Features
- [ ] Microsoft Account login
- [ ] Java Edition crossplay (Geyser/BedrockConnect)
- [ ] Realms support
- [ ] LAN discovery fixes
- [ ] Server browser
- [ ] Direct connect by IP

### Multiplayer QoL
- [ ] Player list (Tab key)
- [ ] Kick/ban commands
- [ ] Whitelist support
- [ ] Server favorites list

---

## Phase 6: Customization

**Goal**: Allow players to customize their experience

### Skins & Textures
- [ ] Custom skins (64x64 Java Edition format)
- [ ] Skin picker UI
- [ ] Online skin downloading (Mojang API)
- [ ] Resource pack support
- [ ] Shader support (SEUS, BSL, etc.)

### UI/UX
- [ ] Classic Java crafting UI (toggle)
- [ ] Trading GUI overhaul
- [ ] Modern launcher
- [ ] In-game settings overhaul
- [ ] FOV slider
- [ ] GUI scale options

---

## Phase 7: Performance & Optimization

**Goal**: Optimize for modern PC hardware

### Performance
- [ ] Multithreading optimization
- [ ] Refactor SPU task system for PC
- [ ] Separate render thread from game logic
- [ ] Async chunk loading
- [ ] Frustum culling improvements
- [ ] Occlusion culling
- [ ] LOD system for distant chunks

### Memory
- [ ] Fix memory leaks
- [ ] Optimize chunk memory usage
- [ ] Implement chunk unloading
- [ ] Reduce texture memory footprint

---

## Phase 8: Quality of Life

**Goal**: Add modern conveniences

### Features
- [ ] Configurable autosave interval
- [ ] Screenshot system (F2)
- [ ] World backup and restore
- [ ] Import/export Java Edition worlds
- [ ] Seed display and copy
- [ ] Chat improvements (history, tab completion, clickable links)

---

## Priority Levels

### P0 (Critical) - Do First
- Fix performance freezes
- Remove world size limits completely
- Fix void/auto-save bugs
- Full mouse navigation

### P1 (High) - Do Soon
- Raw input support
- Creative flight fix
- Custom skins
- Classic crafting UI
- 1.7 world generation

### P2 (Medium) - Do Eventually
- 1.8 blocks/mobs
- Java crossplay
- F3 debug enhancements
- Enchanting/villager overhaul

### P3 (Low) - Nice to Have
- Spectator mode
- Shader support
- Modern backports
- Mod loader integration

---

## Community Requests

Want a feature added? Open an issue on GitHub with the `feature-request` label.

Most requested features will be prioritized.

---

## Development Philosophy

### Core Principles
1. **Preserve Legacy Feel** - Keep 1.8 combat, quasi-connectivity, 4J UI style
2. **Fix Bugs, Keep Glitches** - Fix crashes, preserve fun exploits
3. **PC-First** - Optimize for PC hardware, not console limitations
4. **Optional Modernization** - New features should be toggleable

### What We Won't Change
- ❌ 1.8 Combat (no attack cooldown)
- ❌ Java Redstone (quasi-connectivity, zero-tick)
- ❌ 4J UI Navigation (looping menus)
- ❌ Fun glitches (overstacked items, block transmutation)

---

## Timeline Estimates

| Phase | Estimated Time | Status |
|-------|---------------|--------|
| **Phase 1** | 2-4 weeks | 🔄 In Progress (90%) |
| **Phase 2** | 4-6 weeks | ⏳ Not Started |
| **Phase 3** | 3-6 months | ⏳ Not Started |
| **Phase 4** | 1-2 months | ⏳ Not Started |
| **Phase 5** | 2-3 months | ⏳ Not Started |
| **Phase 6** | 1-2 months | ⏳ Not Started |
| **Phase 7** | 2-4 weeks | ⏳ Not Started |
| **Phase 8** | 1-2 months | ⏳ Not Started |

**Total Estimated Time**: 8-12 months for full roadmap

---

## Contributing

Want to help? Check out:
- [Open Issues](https://github.com/Minecraft-Community-Edition/client/issues)
- [Good First Issues](https://github.com/Minecraft-Community-Edition/client/labels/good%20first%20issue)
- [Contributing Guidelines](https://github.com/Minecraft-Community-Edition/client/blob/main/CONTRIBUTING.md)

---

## Changelog

### March 10, 2026
- ✅ Completed Phase 1 critical bug fixes
- ✅ Added 8 new features
- ✅ Build system configured
- 🔄 Started performance profiling

### March 9, 2026
- ✅ Fixed chest crash bug
- ✅ Fixed void spawn bug
- ✅ Fixed random damage bug
- ✅ Increased TNT and gravity block limits

---

**Last Updated**: March 10, 2026  
**Current Phase**: Phase 1 (90% complete)  
**Next Milestone**: Performance optimization and testing
