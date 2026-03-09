# TODO: Making Whisper LCE Usable on PC

## Critical Fixes (Game-Breaking)

- [ ] **Fix Chest Crash Bug** ✅ COMPLETED
  - NBT item count overflow when stacks exceed 127
  - Fixed by saving Count as short instead of signed byte

- [ ] **Fix Void Spawn Bug** ✅ COMPLETED
  - Players spawning in void when logging in on transparent blocks
  - Fixed by adding solid ground check in spawn validation

- [ ] **Fix Random Damage Bug** ✅ COMPLETED
  - Players taking suffocation damage near walls due to float precision
  - Fixed by increasing isInWall threshold from 1/8 to 3/8 points

- [ ] **Fix Performance Freezes**
  - Game freezes/stutters during chunk loading
  - Investigate multithreading issues on PC
  - Profile with microprofile to identify bottlenecks
  - Possible causes: SPU task system not optimized for PC, synchronous I/O

- [ ] **Fix Lag Blocks (Mining Desync)** ⚠️ KNOWN LIMITATION
  - Client-side optimistic block breaking causes desync on high latency
  - Would require major refactoring to add server confirmation
  - Documented as known limitation for now

## Mouse & Keyboard Support

- [ ] **Full Mouse Navigation**
  - Implement proper mouse cursor rendering (not just controller emulation)
  - Add hover states for all UI elements
  - Click-to-select for menus, inventory, crafting
  - Scroll wheel support for lists and inventory
  - Right-click context menus where appropriate

- [ ] **Sign Text Entry** ⚠️ PARTIALLY BROKEN
  - Current KBM sign input is disabled due to crashes
  - Need to properly implement keyboard text input for signs
  - Fix `UIScene_SignEntryMenu.cpp` keyboard mode

- [ ] **Improved Keyboard Controls**
  - Rebindable keys (currently hardcoded)
  - Standard PC keybinds (E for inventory, ESC for menu, etc.)
  - Hotbar number keys (1-9)
  - F3 debug screen
  - F11 fullscreen toggle

- [ ] **Mouse Sensitivity & Raw Input**
  - Add mouse sensitivity slider
  - Implement raw input (bypass Windows acceleration)
  - Separate X/Y sensitivity options
  - Invert mouse option

## Online Features

- [ ] **Java Edition Crossplay**
  - Implement Geyser/BedrockConnect protocol translation
  - Add server browser for Java servers
  - Handle protocol differences (block IDs, NBT format, etc.)
  - Test with popular Java servers (Hypixel, Mineplex, etc.)

- [ ] **Microsoft Account Login**
  - Integrate Xbox Live authentication in launcher
  - Support both Microsoft and legacy Mojang accounts
  - Store auth tokens securely
  - Auto-refresh expired tokens

- [ ] **Realms Support**
  - Connect to Minecraft Realms servers
  - Implement Realms API integration
  - Handle Realms-specific features (backups, invites, etc.)

- [ ] **LAN Discovery**
  - Fix LAN game discovery on PC networks
  - Broadcast game sessions properly
  - Handle Windows Firewall prompts

## Customization

- [ ] **Custom Skins**
  - Support standard 64x64 Java Edition skins
  - Implement skin picker UI
  - Allow loading skins from file
  - Support slim/classic arm models
  - Online skin downloading from Mojang API

- [ ] **Resource Pack Support**
  - Load Java Edition resource packs
  - Convert textures to Console Edition format
  - Support custom sounds and music
  - UI for enabling/disabling packs

- [ ] **Shader Support**
  - Integrate basic shader support (SEUS, BSL, etc.)
  - Add shader options menu
  - Optimize for PC hardware (not console SPU)

## Content Updates

- [ ] **Update to 1.8.9**
  - Add missing 1.8.9 blocks (prismarine variants, sea lanterns, etc.)
  - Implement ocean monuments
  - Add guardians and elder guardians
  - Update world generation for ocean monuments
  - Add 1.8.9 items (prismarine shards, crystals, etc.)
  - Update crafting recipes
  - Implement dual-wielding (1.9 feature but commonly requested)

- [ ] **Backport Modern Features (Optional)**
  - Elytra flight (1.9)
  - Shulker boxes (1.11)
  - Concrete blocks (1.12)
  - Tridents and underwater combat (1.13)
  - Note: Keep as optional to preserve 1.8 feel

## UI/UX Improvements

- [ ] **Modern Launcher**
  - Replace old 4J launcher with custom PC launcher
  - Profile management (multiple accounts)
  - Version selection (if multiple builds exist)
  - Mod loader integration (Forge/Fabric compatibility layer?)
  - News feed and update notifications

- [ ] **In-Game Settings Overhaul**
  - Separate video, audio, controls, and gameplay tabs
  - Advanced graphics options (render distance, particles, etc.)
  - FOV slider (currently locked to console FOV)
  - GUI scale options
  - Accessibility options (narrator, high contrast, etc.)

- [ ] **Chat Improvements**
  - Proper keyboard chat input (not controller)
  - Chat history (up arrow to recall)
  - Tab completion for commands and usernames
  - Clickable links and coordinates
  - Chat formatting codes support

- [ ] **F3 Debug Screen**
  - Show coordinates, biome, light level
  - FPS counter and performance metrics
  - Chunk updates and entity count
  - Memory usage
  - Toggle with F3 key

## Performance Optimizations

- [ ] **Multithreading Optimization**
  - Refactor SPU task system for PC CPU architecture
  - Separate render thread from game logic thread
  - Async chunk loading and generation
  - Profile and optimize critical paths

- [ ] **Render Optimizations**
  - Implement frustum culling improvements
  - Occlusion culling for underground areas
  - LOD system for distant chunks
  - Optimize particle rendering
  - VSync and frame limiter options

- [ ] **Memory Management**
  - Fix memory leaks (if any)
  - Optimize chunk memory usage
  - Implement chunk unloading for distant areas
  - Reduce texture memory footprint

## Quality of Life

- [ ] **Autosave Improvements**
  - Configurable autosave interval
  - Save indicator (so players know when it's safe to quit)
  - Backup system (keep last N saves)

- [ ] **Screenshot System**
  - F2 to take screenshot
  - Save to dedicated screenshots folder
  - Show notification when screenshot taken
  - Support for high-res screenshots

- [ ] **World Management**
  - Import Java Edition worlds
  - Export to Java Edition format
  - World backup and restore
  - Seed display and copy

- [ ] **Multiplayer QoL**
  - Server favorites list
  - Direct connect by IP
  - Player list (Tab key)
  - Kick/ban commands for hosts
  - Whitelist support

## Bug Fixes (Non-Critical)

- [ ] **Fix Block Transmutation Glitches**
  - Make safe so they don't corrupt saves
  - Keep as "fun glitches" but prevent crashes
  - Document known glitches for speedrunners

- [ ] **Fix Overstacked Items**
  - Allow overstacked items but prevent duplication exploits
  - Ensure they save/load correctly (already fixed with chest crash)

- [ ] **Audio Issues**
  - Fix missing/incorrect sounds
  - Implement proper 3D audio positioning
  - Add music volume slider
  - Fix sound cutting out issues

## Documentation

- [ ] **User Manual**
  - Installation guide
  - Controls reference
  - Known issues and workarounds
  - FAQ

- [ ] **Developer Documentation**
  - Build instructions (already partially done)
  - Code architecture overview
  - Modding API documentation
  - Contributing guidelines

- [ ] **Changelog**
  - Document all changes from original LCE
  - Version history
  - Planned features roadmap

## Testing & Stability

- [ ] **Automated Testing**
  - Unit tests for critical systems
  - Integration tests for multiplayer
  - Performance benchmarks
  - Regression testing for bug fixes

- [ ] **Platform Testing**
  - Test on Windows 10/11
  - Test on various hardware configs
  - Test with different graphics cards (NVIDIA, AMD, Intel)
  - Verify compatibility with Windows 7/8 (if desired)

## Legal & Distribution

- [ ] **License Compliance**
  - Ensure all third-party libraries are properly licensed
  - Add license file and attributions
  - Clarify legal status (requires original game files?)

- [ ] **Distribution Method**
  - GitHub releases with pre-built binaries
  - Installer/updater system
  - Mod repository (if modding support added)

---

## Priority Levels

**P0 (Critical)**: Performance freezes, lag blocks investigation
**P1 (High)**: Full mouse navigation, custom skins, Microsoft login, 1.8.9 update
**P2 (Medium)**: Java crossplay, resource packs, UI improvements, F3 debug
**P3 (Low)**: Shader support, modern feature backports, advanced customization

---

## Notes

- Keep Legacy Console Edition feel and mechanics (1.8 combat, quasi-connectivity, 4J UI style)
- Preserve "fun glitches" where possible (overstacked items, block transmutation) but make them safe
- Focus on PC-specific improvements without breaking console compatibility
- Consider creating a "Legacy+" mode with optional modern features
