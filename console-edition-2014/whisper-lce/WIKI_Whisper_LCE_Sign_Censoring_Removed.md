# Sign Censoring Removed

## Summary
Completely disabled sign text censoring/profanity filtering. All signs now display exactly as written - **free speech enabled!**

## Changes Made

### SignTileEntity.cpp - Censoring Disabled

**1. StringVerifyCallback() - Line 159**
```cpp
// BEFORE:
pClass->m_bCensored=false;
for(int i=0;i<pResults->wNumStrings;i++)
{
    if(pResults->pStringResult[i]!=ERROR_SUCCESS)
    {
        pClass->m_bCensored=true;  // Marked as censored if profanity detected
    }
}

// AFTER:
pClass->m_bCensored=false;  // Always false - no censoring
// Removed profanity checking loop entirely
```

**2. setChanged() - Line 120**
```cpp
// BEFORE:
if(!m_bVerified)
{
    // Called InputManager.VerifyStrings() to check for profanity
    // Waited for callback to mark as verified/censored
}

// AFTER:
m_bVerified = true;   // Immediately verified
m_bCensored = false;  // Never censored
// Skip string verification entirely
```

## What This Does

### Before (Censored):
1. Player writes text on sign
2. Text sent to InputManager.VerifyStrings()
3. System checks against profanity filter
4. If profanity detected: `m_bCensored = true`
5. Censored signs displayed with asterisks/blocked text
6. Other players see censored version

### After (Free Speech):
1. Player writes text on sign
2. Text immediately marked as verified
3. **No profanity checking**
4. `m_bCensored` always stays `false`
5. Signs display exactly as written
6. All players see original text

## Impact

### What's Changed:
- ✅ **No profanity filtering** on signs
- ✅ **No asterisks** replacing "bad words"
- ✅ **No censored text** warnings
- ✅ **Instant sign updates** (no verification delay)
- ✅ **All text displays** exactly as typed

### What Still Works:
- ✅ Sign text length limits (still enforced)
- ✅ Sign editing and placement
- ✅ Sign text synchronization in multiplayer
- ✅ Sign saving/loading

## Technical Details

### Censoring System (Now Disabled):
The original system used:
- `InputManager.VerifyStrings()` - Platform profanity filter
- `STRING_VERIFY_RESPONSE` - Results from filter
- `m_bCensored` flag - Marked censored signs
- `StringVerifyCallback()` - Processed filter results

### New Behavior:
```cpp
// Signs are always:
m_bVerified = true;   // Verified immediately
m_bCensored = false;  // Never censored

// No calls to:
InputManager.VerifyStrings()  // Skipped
StringVerifyCallback()        // Still exists but always returns false
```

## Platform Differences

### Original Console Behavior:
- **Xbox 360/One**: Used Xbox Live profanity filter
- **PlayStation**: Used PSN profanity filter  
- **Different regions**: Different word lists
- **Age ratings**: Stricter for younger accounts

### New Behavior (All Platforms):
- **No filtering** on any platform
- **Same behavior** everywhere
- **No regional differences**
- **No age-based restrictions**

## Multiplayer Considerations

### Host/Client:
- **Host creates sign**: No censoring, sent to clients as-is
- **Client creates sign**: No censoring, sent to host as-is
- **All players see**: Exact same text

### Network Protocol:
- `SignUpdatePacket` still includes `m_bCensored` field
- Field is always `false` now
- Backward compatible with old clients (they just see uncensored text)

## Use Cases

### Creative Freedom:
- ✅ Write any text on signs
- ✅ Use slang, abbreviations, internet speak
- ✅ No false positives from overzealous filters
- ✅ Express yourself freely

### Common False Positives (Now Fixed):
- "Assassin" - Often censored (contains "ass")
- "Scunthorpe" - Classic false positive
- "Penistone" - UK town name
- "Dickens" - Author name
- Many legitimate words with substring matches

## Comparison

| Feature | Before | After |
|---------|--------|-------|
| Profanity Filter | ✅ Active | ❌ Disabled |
| Censored Text | ✅ Asterisks | ❌ None |
| Verification Delay | ~1 second | Instant |
| False Positives | Common | None |
| Regional Differences | Yes | No |
| Age Restrictions | Yes | No |

## Code Locations

**Files Modified**:
- `Minecraft.World/SignTileEntity.cpp` (lines 120-157, 159-180)

**Functions Changed**:
- `SignTileEntity::setChanged()` - Skip verification
- `SignTileEntity::StringVerifyCallback()` - Always return false

**Flags Always Set**:
- `m_bVerified = true`
- `m_bCensored = false`

## Testing

### To Test:
1. Place a sign
2. Write any text (including "profanity")
3. Exit sign editor
4. Check sign displays exact text
5. In multiplayer: Other players see same text
6. Save/reload world: Text persists uncensored

### Expected Results:
- ✅ All text displays as written
- ✅ No asterisks or censoring
- ✅ Instant updates (no delay)
- ✅ Works in multiplayer
- ✅ Persists through save/load

## Notes

### Why This Matters:
- **Creative freedom**: Build without restrictions
- **No false positives**: Legitimate words work
- **Faster**: No verification delay
- **Consistent**: Same everywhere
- **Adult players**: Can use adult language

### Responsibility:
- This is a **single-player/LAN** modification
- Players are responsible for their own content
- Not suitable for public servers with children
- Use responsibly in multiplayer

### Original Purpose:
The censoring was added for:
- **Console certification**: Required by Xbox/PlayStation
- **Age ratings**: ESRB/PEGI compliance
- **Online safety**: Protecting younger players
- **Terms of Service**: Platform requirements

Since this is a **modified version** for personal use, these restrictions no longer apply.

---

**Difficulty**: Easy (2/10)  
**Time to Implement**: 5 minutes  
**Risk Level**: Very Low  
**Impact**: High (quality of life)  
**Recommended**: Yes for adult players

🗣️ **Free Speech Enabled!** 🗣️
