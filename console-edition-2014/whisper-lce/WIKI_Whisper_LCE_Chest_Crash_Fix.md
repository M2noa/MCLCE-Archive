# Chest Crash Fix - Overstacked Items (255+ Stacks)

## The Problem

In later versions of Legacy Console Edition, opening a chest containing overstacked items (like a stack of 255) would instantly crash the game. This was caused by improper handling of item counts in the NBT save/load system.

## Root Cause

**File:** `Minecraft.World\ItemInstance.cpp`

The original code saved item counts as a **signed byte** (-128 to 127):
```cpp
compoundTag->putByte(L"Count", (byte) count);  // Line 142
count = compoundTag->getByte(L"Count");        // Line 152
```

When an item stack had a count of 255:
- It would be cast to a signed byte, becoming **-1**
- When loading, this negative value would cause crashes or corruption
- The UI would fail to render the item properly

## The Fix

Changed item count storage from **byte** to **short** (supports -32,768 to 32,767):

```cpp
// Save as short instead of byte
compoundTag->putShort(L"Count", (short) count);

// Load as short, with fallback for old saves
count = compoundTag->getShort(L"Count");
if (count == 0)
{
    count = (unsigned char)compoundTag->getByte(L"Count");
}
```

## Benefits

1. **No More Crashes**: Chests with overstacked items (65-255) can be opened safely
2. **Backward Compatible**: Old saves with byte counts still load correctly
3. **Future-Proof**: Supports stacks up to 32,767 (though game logic limits to 255)
4. **Preserves Glitches**: Players can still obtain overstacked items via furnace dupe or other methods

## Technical Details

- **Modified File**: `Minecraft.World\ItemInstance.cpp` (lines 139-158)
- **Save Format**: Changed from `putByte` to `putShort`
- **Load Format**: Tries `getShort` first, falls back to `getByte` for old saves
- **Data Type**: `count` remains an `int` in memory, only storage format changed

## Testing

To verify the fix works:
1. Use furnace dupe to create a stack of 255 items
2. Place the stack in a chest
3. Close and reopen the chest
4. **Expected**: Chest opens without crashing, items display correctly
5. **Before Fix**: Game would crash immediately when opening the chest

## Related Fixes

This fix is part of the "Keep it, but Make it Safe" philosophy:
- ✅ Overstacked items (65-255) are preserved
- ✅ Understacked items (0 count) still work
- ✅ No artificial limits added
- ✅ Game engine handles edge cases gracefully

## Notes

- This does NOT remove the ability to get overstacked items
- Furnace dupe and other methods still work
- The fix only prevents crashes when storing/loading these items
- UI rendering already handled large counts correctly
