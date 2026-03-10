# Features Added

This page documents all new features and enhancements added to Whisper LCE by M1noa.

## Overview

Whisper LCE includes several PC-specific features and enhancements not present in the original Console Edition:

- Enhanced Debug Menu (F3)
- Infinite World Size
- Sign Censoring Removed
- Chat System Re-enabled
- Professional Launcher
- Increased Entity Limits

---

## 1. Enhanced Debug Menu (F3) ✅

**Status**: Fully Implemented

### Features

- **Resizable Window** - 600×900 default size, fully resizable
- **Scrollable Interface** - Handle large amounts of debug info
- **Command Console** - Built-in command input
- **Command History** - View previous commands and outputs
- **Real-time Updates** - Live debug information

### Supported Commands

| Command | Description | Example |
|---------|-------------|---------|
| `/tp` | Teleport player | `/tp 100 64 100` |
| `/tp` (relative) | Relative teleport | `/tp ~10 ~5 ~-10` |
| `/gamemode` | Change gamemode | `/gamemode 1` (creative) |
| `/time` | Set world time | `/time set day` |
| `/give` | Give items | `/give 1 64` (64 stone) |
| `/kill` | Kill player | `/kill` |

### Usage

1. Press **F3** to open debug menu
2. Type command in input box
3. Press **Send** or **Enter**
4. View output in history panel

### Technical Details

- **Implementation**: ImGui-based window
- **Input Handling**: WM_CHAR keyboard events
- **Command Parsing**: Custom parser with relative coordinate support
- **Thread Safety**: Commands executed on main game thread

---

## 2. Infinite World Size ✅

**Status**: Fully Implemented

### Changes

| Dimension | Original Size | New Size | Increase |
|-----------|--------------|----------|----------|
| **Overworld** | 864 blocks (54 chunks) | 32,768 blocks (2048 chunks) | 38× |
| **Nether** | 108 blocks (scaled 8:1) | 4,096 blocks (scaled 8:1) | 38× |
| **The End** | 288 blocks (18 chunks) | 288 blocks (18 chunks) | No change |

### Features

- ✅ Removed invisible bedrock walls at world border
- ✅ Removed water moat around world edge
- ✅ Proper chunk generation at extended distances
- ✅ Nether portal linking works correctly
- ✅ The End stays fixed size (central island only)

### Technical Details

**Files Modified**:
- World generation constants
- Chunk loading system
- Border rendering code
- Nether portal linking

**Code Changes**:
```cpp
// Before
#define WORLD_SIZE_CHUNKS 54
#define WORLD_SIZE_BLOCKS 864

// After
#define WORLD_SIZE_CHUNKS 2048
#define WORLD_SIZE_BLOCKS 32768
```

### Limitations

- The End remains 18 chunks (288 blocks) - intentional design
- Outer End islands not implemented (Console Edition limitation)
- Performance may degrade at extreme distances (>16,000 blocks)

**Details**: See [INFINITE_WORLD_SIZE_CHANGES.md](https://github.com/Minecraft-Community-Edition/client/blob/main/INFINITE_WORLD_SIZE_CHANGES.md)

---

## 3. Sign Censoring Removed ✅

**Status**: Fully Implemented

### Changes

- ✅ All profanity filtering disabled
- ✅ Signs display text exactly as written
- ✅ No asterisks or blocked text
- ✅ Instant sign updates (no verification delay)
- ✅ Full creative freedom

### Original Behavior

The original Console Edition had:
- Server-side profanity filter
- Blocked words replaced with asterisks
- Delay while text was verified
- Limited character set

### New Behavior

- No filtering whatsoever
- Instant text display
- Full Unicode support (if font supports it)
- No network verification delay

### Technical Details

**Files Modified**:
- Sign text validation code
- Network packet handling
- Text rendering system

**Code Changes**:
```cpp
// Before
if (containsProfanity(text)) {
    text = censorText(text);
}

// After
// Censoring code removed entirely
```

**Details**: See [SIGN_CENSORING_REMOVED.md](https://github.com/Minecraft-Community-Edition/client/blob/main/SIGN_CENSORING_REMOVED.md)

---

## 4. Chat System Re-enabled ✅

**Status**: Fully Implemented

### Features

- ✅ Press **T** to open chat
- ✅ Full keyboard input support
- ✅ Standard Minecraft chat experience
- ✅ Chat history (up arrow to recall)
- ✅ Command support (starts with `/`)

### Original Behavior

The original Console Edition PC build had:
- Chat disabled entirely
- No keyboard input handling
- Controller-only text entry

### New Behavior

- Full keyboard chat input
- WM_CHAR event handling restored
- Standard PC controls
- Seamless typing experience

### Technical Details

**Files Modified**:
- Chat input handling
- Keyboard event processing
- UI rendering

**Code Changes**:
```cpp
// Restored WM_CHAR handler
case WM_CHAR:
    if (chatOpen) {
        handleChatInput(wParam);
    }
    break;
```

### Usage

1. Press **T** to open chat
2. Type your message
3. Press **Enter** to send
4. Press **Escape** to cancel

---

## 5. Professional Launcher ✅

**Status**: Fully Implemented

### Features

- **Debug Console Toggle** - Enable/disable console window
- **Debug Menu Toggle** - Enable/disable F3 menu
- **Persistent Settings** - Saved to `launcher_config.ini`
- **Command-line Args** - `--debug-console`, `--debug-menu`
- **Fixes Instant-Close** - Game no longer closes immediately

### Configuration File

`launcher_config.ini`:
```ini
[Settings]
DebugConsole=1
DebugMenu=1
```

### Command-Line Usage

```batch
# Enable debug console
Launcher.exe --debug-console

# Enable debug menu
Launcher.exe --debug-menu

# Enable both
Launcher.exe --debug-console --debug-menu
```

### Technical Details

**Files**:
- `Launcher/Launcher.cpp` - Main launcher code
- `Launcher/launcher_config.ini` - Settings file
- `Launcher/README.md` - Documentation

**Size**: 51KB (Release build)

**Features**:
- INI file parsing
- Command-line argument parsing
- Process spawning with arguments
- Settings persistence

**Details**: See [Launcher/README.md](https://github.com/Minecraft-Community-Edition/client/blob/main/Launcher/README.md)

---

## 6. Increased Entity Limits ✅

**Status**: Fully Implemented

### TNT Limit Increase

| Metric | Original | New | Increase |
|--------|----------|-----|----------|
| **Max TNT** | 20 | 100 | 5× |
| **Despawn Behavior** | Despawns when looking away | Never despawns | Removed |

**Impact**:
- Larger TNT cannons possible
- Massive explosions work properly
- No more vanishing TNT

### Gravity Block Limit Increase

| Metric | Original | New | Increase |
|--------|----------|-----|----------|
| **Max Falling Blocks** | 20 | 200 | 10× |
| **Freeze Behavior** | Freezes mid-air | Falls properly | Fixed |

**Impact**:
- Massive sand/gravel structures fall correctly
- No more frozen blocks
- Better physics simulation

### Technical Details

**Files Modified**:
- `Minecraft.Client\ServerLevel.h`

**Code Changes**:
```cpp
// TNT Limit
#define MAX_PRIMED_TNT 100  // was 20

// Gravity Block Limit
#define MAX_FALLING_BLOCKS 200  // was 20
```

---

## 7. Sign Keyboard Support (Disabled) ⚠️

**Status**: Implemented but Disabled

### Why Disabled?

Direct keyboard typing on signs was implemented but disabled due to build dependencies:
- `g_KBMInput` not available in Windows build
- Missing keyboard/mouse input detection
- Would require additional includes and refactoring

### Original Implementation

- Keyboard/mouse: Direct typing (like chat)
- Controller: Auto-popup on-screen keyboard
- Smart input detection
- Shortcuts: Enter (save), Escape (cancel), Backspace (delete)

### Current Behavior

- All input uses on-screen keyboard
- Works with both controller and keyboard
- Less convenient but functional

### Future Plans

Can be re-enabled later with proper includes and input detection system.

**Details**: See [SIGN_KEYBOARD_SUPPORT.md](https://github.com/Minecraft-Community-Edition/client/blob/main/SIGN_KEYBOARD_SUPPORT.md)

---

## Feature Comparison Table

| Feature | Original Console Edition | Whisper LCE (M1noa) |
|---------|-------------------------|---------------------|
| **Debug Menu** | ❌ None | ✅ F3 menu with commands |
| **World Size** | 864 blocks | 32,768 blocks (38×) |
| **Sign Censoring** | ✅ Active | ❌ Removed |
| **Chat System** | ❌ Disabled on PC | ✅ Full keyboard support |
| **Launcher** | ❌ None | ✅ Custom launcher |
| **TNT Limit** | 20 | 100 (5×) |
| **Gravity Blocks** | 20 | 200 (10×) |
| **Sign Keyboard** | ❌ None | ⚠️ Implemented but disabled |

---

## Statistics

| Metric | Value |
|--------|-------|
| **Features Added** | 8 |
| **Features Fully Working** | 7 |
| **Features Disabled** | 1 |
| **Code Changes** | ~1,600 lines |
| **Documentation** | 18 files (~95KB) |

---

## Future Features (Planned)

See [TODO.md](https://github.com/Minecraft-Community-Edition/client/blob/main/TODO.md) for complete roadmap.

### High Priority
- Full mouse navigation for menus
- Fix performance freezes
- Custom skins support
- Resource pack support

### Medium Priority
- 1.7 world generation (new biomes)
- 1.8 blocks and items
- Java Edition crossplay
- Shader support

### Low Priority
- Spectator mode
- Modern feature backports (Elytra, Shulker boxes, etc.)
- Mod loader integration

---

**Last Updated**: March 10, 2026  
**Features Implemented**: 8  
**Status**: Ready for testing
