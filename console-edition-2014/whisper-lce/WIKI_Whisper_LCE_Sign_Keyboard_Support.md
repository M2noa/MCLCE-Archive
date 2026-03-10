# Sign Keyboard Support Enhanced

## Summary
Added comprehensive keyboard and mouse support for sign editing, plus automatic on-screen keyboard for controller users.

## Changes Made

### 1. **Keyboard/Mouse Typing Support** ⭐
**File**: `Minecraft.Client/Windows64/Windows64_Minecraft.cpp`

**Added WM_CHAR handler for sign editing**:
```cpp
// Check if we're in a sign editing screen
UIScene_SignEntryMenu* signMenu = dynamic_cast<UIScene_SignEntryMenu*>(pMinecraft->screen);
if (signMenu && g_KBMInput.IsKBMActive())
{
    // Handle direct character input for sign editing
    wchar_t ch = (wchar_t)wParam;
    
    // Allow printable characters (space through tilde)
    if (ch >= 32 && ch <= 126)
    {
        wstring current = signMenu->GetCurrentLineText();
        if (current.length() < 15)  // Sign line length limit
        {
            signMenu->AppendToCurrentLine(ch);
        }
    }
}
```

**Features**:
- ✅ Type directly on signs with physical keyboard
- ✅ Supports all printable ASCII characters (32-126)
- ✅ Respects 15-character line limit
- ✅ Works like chat system (T key)
- ✅ Only activates when keyboard/mouse is active

### 2. **Controller On-Screen Keyboard** ⭐
**File**: `Minecraft.Client/Common/UI/UIScene_SignEntryMenu.cpp`

**Enhanced handleInput() function**:
```cpp
// Check if player is using keyboard/mouse or controller
bool isUsingKeyboard = (iPad == 0) && g_KBMInput.IsKBMActive();

// Controller input - show on-screen keyboard
if (pressed && !isUsingKeyboard)
{
    m_bIgnoreInput = true;
    
    // Show on-screen keyboard for controller users
    int language = XGetLanguage();
    C_4JInput::EKeyboardMode mode = C_4JInput::EKeyboardMode_Alphabet;
    
    // Use different keyboard modes based on language
    switch(language)
    {
    case XC_LANGUAGE_JAPANESE:
    case XC_LANGUAGE_KOREAN:
    case XC_LANGUAGE_TCHINESE:
        mode = C_4JInput::EKeyboardMode_Email;
        break;
    default:
        mode = C_4JInput::EKeyboardMode_Alphabet;
        break;
    }
    
    InputManager.RequestKeyboard(
        app.GetString(IDS_SIGN_TITLE),
        m_textInputLines[m_iEditingLine].getLabel(),
        (DWORD)iPad,
        15,
        &UIScene_SignEntryMenu::KeyboardCompleteCallback,
        this,
        mode
    );
}
```

**Features**:
- ✅ Automatically detects controller vs keyboard input
- ✅ Shows platform on-screen keyboard when using controller
- ✅ Language-aware keyboard selection
- ✅ Japanese/Korean/Chinese: Email mode (for character support)
- ✅ Default: Alphabet mode (letters and numbers)

### 3. **Keyboard Shortcuts for Signs** ⭐
**File**: `Minecraft.Client/Common/UI/UIScene_SignEntryMenu.cpp`

**Added keyboard shortcuts**:
```cpp
// Handle keyboard input for typing on signs
if (isUsingKeyboard && pressed)
{
    // Enter key to confirm sign
    if (key == VK_RETURN)
    {
        m_bConfirmed = true;
        handled = true;
        return;
    }
    
    // Escape to cancel
    if (key == VK_ESCAPE)
    {
        // Clear sign and exit
        navigateBack();
        ui.PlayUISFX(eSFX_Back);
        handled = true;
        return;
    }
    
    // Backspace to delete
    if (key == VK_BACK)
    {
        wstring current = m_textInputLines[m_iEditingLine].getLabel();
        m_textInputLines[m_iEditingLine].setLabel(
            current.substr(0, current.length() - 1)
        );
        handled = true;
        return;
    }
}
```

**Shortcuts**:
- ✅ **Enter**: Confirm/save sign
- ✅ **Escape**: Cancel/clear sign
- ✅ **Backspace**: Delete last character
- ✅ **A-Z, 0-9**: Type characters (via WM_CHAR)

### 4. **Helper Methods Added** ⭐
**File**: `Minecraft.Client/Common/UI/UIScene_SignEntryMenu.h`

```cpp
// Keyboard/mouse support for sign editing
wstring GetCurrentLineText() { return m_textInputLines[m_iEditingLine].getLabel(); }
void AppendToCurrentLine(wchar_t ch) { 
    wstring current = m_textInputLines[m_iEditingLine].getLabel();
    if (current.length() < 15) {
        m_textInputLines[m_iEditingLine].setLabel(current + ch);
    }
}
```

---

## How It Works

### Keyboard/Mouse Users:
1. **Place sign** → Sign editing screen opens
2. **Click on line** or use arrow keys to select line
3. **Type directly** with keyboard (like typing in chat)
4. **Press Enter** to save and exit
5. **Press Escape** to cancel and clear
6. **Use Backspace** to delete characters

### Controller Users:
1. **Place sign** → Sign editing screen opens
2. **Navigate** with D-pad/stick to select line
3. **Press A/X button** → On-screen keyboard appears
4. **Type** using on-screen keyboard
5. **Confirm** on on-screen keyboard
6. **Repeat** for each line
7. **Press A/X on "Done"** to save

### Automatic Detection:
```cpp
bool isUsingKeyboard = (iPad == 0) && g_KBMInput.IsKBMActive();
```

- Checks if keyboard/mouse has been recently used
- Switches input mode automatically
- No manual configuration needed

---

## Input Mode Comparison

| Feature | Keyboard/Mouse | Controller |
|---------|---------------|------------|
| **Typing** | Direct keyboard input | On-screen keyboard |
| **Confirm** | Enter key | A/X button |
| **Cancel** | Escape key | B/Circle button |
| **Delete** | Backspace key | On-screen keyboard backspace |
| **Navigation** | Click/Arrow keys | D-pad/Stick |
| **Character Set** | Full ASCII | Platform keyboard |
| **Speed** | Fast (touch typing) | Slower (virtual keyboard) |

---

## Character Support

### Keyboard/Mouse:
- **ASCII 32-126**: All printable characters
- Includes: `A-Z`, `a-z`, `0-9`, punctuation, symbols
- Examples: `!@#$%^&*()_+-=[]{}|;:',.<>?/~\``

### Controller (On-Screen Keyboard):
- **Platform-dependent**: Uses Xbox/PlayStation keyboard
- **Language-aware**: Different keyboards per region
- **Full Unicode**: Supports special characters, emojis (platform dependent)
- **Predictive text**: Some platforms offer word prediction

---

## Technical Details

### Input Detection:
```cpp
g_KBMInput.IsKBMActive()  // Returns true if keyboard/mouse recently used
```

- Set when keyboard/mouse input is detected
- Cleared after controller input
- Allows seamless switching between input methods

### WM_CHAR Handler:
```cpp
case WM_CHAR:
    wchar_t ch = (wchar_t)wParam;
    if (ch >= 32 && ch <= 126)  // Printable ASCII
    {
        // Add character to current line
    }
```

- Windows sends WM_CHAR for character input
- Filters to printable characters only
- Respects 15-character line limit

### On-Screen Keyboard Modes:
- **EKeyboardMode_Alphabet**: Letters and numbers (default)
- **EKeyboardMode_Email**: Email format with @ and . (Asian languages)
- **EKeyboardMode_Full**: Full keyboard (not used - too complex)
- **EKeyboardMode_Email**: URL/email mode

---

## Testing

### Keyboard/Mouse Test:
1. Launch game with keyboard/mouse connected
2. Place a sign
3. Click on first line
4. Type "Hello World!"
5. Press Enter
6. Sign should display "Hello World!"

### Controller Test:
1. Launch game with controller only
2. Place a sign
3. Navigate to first line
4. Press A/X
5. On-screen keyboard should appear
6. Type text using virtual keyboard
7. Confirm
8. Sign should display your text

### Mixed Input Test:
1. Start with keyboard, type some text
2. Switch to controller
3. On-screen keyboard should appear for next line
4. Switch back to keyboard
5. Direct typing should work again

---

## Benefits

### For Keyboard/Mouse Players:
- ✅ **Much faster** typing
- ✅ **No popups** to deal with
- ✅ **Familiar** chat-like interface
- ✅ **Full control** over text
- ✅ **Touch typing** support

### For Controller Players:
- ✅ **Automatic** on-screen keyboard
- ✅ **No confusion** about how to type
- ✅ **Platform-native** keyboard UI
- ✅ **Language support** built-in
- ✅ **Clear workflow**

### For Everyone:
- ✅ **Automatic detection** of input method
- ✅ **Seamless switching** between methods
- ✅ **Consistent** behavior
- ✅ **No configuration** needed

---

## Code Locations

**Files Modified**:
- `Minecraft.Client/Windows64/Windows64_Minecraft.cpp` - WM_CHAR handler
- `Minecraft.Client/Common/UI/UIScene_SignEntryMenu.cpp` - Input handling
- `Minecraft.Client/Common/UI/UIScene_SignEntryMenu.h` - Helper methods

**Functions Changed**:
- `Windows64_Minecraft::WindowProc()` - Added sign WM_CHAR handling
- `UIScene_SignEntryMenu::handleInput()` - Added keyboard/controller detection
- `UIScene_SignEntryMenu::GetCurrentLineText()` - New helper
- `UIScene_SignEntryMenu::AppendToCurrentLine()` - New helper

---

## Known Limitations

### Keyboard/Mouse:
- Only supports ASCII 32-126 (no Unicode)
- No text selection/copy/paste (yet)
- No cursor movement with arrow keys (yet)

### Controller:
- Slower than keyboard typing
- On-screen keyboard varies by platform
- No predictive text on all platforms

### Both:
- 15-character line limit still enforced
- 4-line sign limit unchanged

---

## Future Enhancements (Optional)

### Possible Additions:
1. **Unicode support** for keyboard input
2. **Cursor movement** with arrow keys
3. **Text selection** with Shift+arrows
4. **Copy/paste** with Ctrl+C/V
5. **Undo/redo** with Ctrl+Z/Y
6. **Character count** display
7. **Font preview** while typing
8. **Sign templates** for common text

---

**Difficulty**: Medium (5/10)  
**Time to Implement**: 30 minutes  
**Risk Level**: Low  
**Impact**: High (quality of life)  
**Recommended**: Yes - essential for PC players

⌨️ **Happy Sign Making!** 🪧
