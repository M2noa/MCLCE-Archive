# Build Troubleshooting

This page covers common build issues and their solutions.

## ✅ FIXED: Missing Header Files

All required header files have been copied to the 4JLibs projects:
- `4J_Input.h` → Input/
- `4J_Storage.h` → Storage/
- `4J_Render.h` → Render/
- `4J_Profile.h` → Profile/
- `extraX64.h` → Profile/ and Storage/

**Status**: Header file errors should now be resolved.

---

## ⚠️ REMAINING ISSUE: Compiler Version Mismatch

### The Problem

Some pre-built .lib files in the project were compiled with **Visual Studio 2013 (v120, _MSC_VER=1700)** but you're using **Visual Studio 2022 (v143, _MSC_VER=1900)**.

### Error Messages
```
mismatch detected for '_MSC_VER': value '1700' doesn't match value '1900'
invalid or corrupt file: cannot read at 0xEF38
unresolved external symbol __iob_func
unresolved external symbol SurvivalMode::SurvivalMode
unresolved external symbol CreativeMode::CreativeMode
```

### Solutions

#### Option 1: Clean Rebuild (Recommended)

1. **Clean the solution**
   - Build → Clean Solution
   
2. **Delete all intermediate files**
   ```
   Delete these folders:
   - x64/
   - Minecraft.World/x64/
   - Minecraft.Client/x64/
   - Windows_Libs/Dev/*/x64/
   - Launcher/x64/
   ```

3. **Rebuild everything**
   - Build → Rebuild Solution
   - This will rebuild all .lib files with VS2022

#### Option 2: Find and Delete Old .lib Files

Search for and delete any pre-built .lib files that were compiled with VS2013:

```batch
cd C:\Users\Minoa\Documents\GitHub\Minecraft\console-edition-2014\whisper-lce
del /s /q *.lib
```

Then rebuild the solution.

#### Option 3: Build Projects Individually

Build in this specific order:

1. Right-click **Minecraft.World** → Build
2. Right-click **Input** → Build
3. Right-click **Storage** → Build  
4. Right-click **Render** → Build
5. Right-click **Profile** → Build
6. Right-click **Minecraft.Client** → Build
7. Right-click **Launcher** → Build

---

## 🔍 Verification

After building, verify these .lib files exist with VS2022:

```
Windows_Libs\Dev\Input\x64\Release\Input.lib
Windows_Libs\Dev\Storage\x64\Release\Storage.lib
Windows_Libs\Dev\Render\x64\Release\Render.lib
Windows_Libs\Dev\Profile\x64\Release\Profile.lib
Minecraft.World\x64\Release\Minecraft.World.lib
```

---

## 📋 Build Checklist

- [x] 4JLibs cloned from GitHub
- [x] Header files copied (4J_*.h, extraX64.h)
- [x] Projects added to solution
- [ ] Clean solution
- [ ] Delete old .lib files
- [ ] Rebuild solution
- [ ] Verify all .lib files created
- [ ] Final build succeeds

---

## 🚀 Expected Result

After cleaning and rebuilding:
- All projects compile with VS2022 (v143)
- All .lib files match compiler version
- Linker errors resolved
- Game builds successfully

---

## Common Errors

### Error: LNK2019 - Unresolved External Symbol

**Cause**: Missing .lib file or incorrect library path

**Solution**:
1. Verify all .lib files exist
2. Check project dependencies
3. Rebuild dependency projects first

### Error: C1083 - Cannot Open Include File

**Cause**: Missing header files

**Solution**:
1. Run `install_4jlibs.bat`
2. Verify header files in `Windows_Libs/Dev/*/inc/`
3. Check include directories in project properties

### Error: MSB8020 - Build Tools Not Found

**Cause**: Wrong platform toolset selected

**Solution**:
1. Right-click project → Properties
2. Configuration Properties → General
3. Platform Toolset → v143 (VS 2022)
4. Apply and rebuild

### Error: Out of Memory

**Cause**: MSBuild CLI memory limits

**Solution**:
- Always build in Visual Studio IDE, not command line
- Close other applications
- Increase virtual memory

---

## Performance Tips

### Faster Builds

1. **Use SSD** - Store project on SSD for faster I/O
2. **Disable Antivirus** - Temporarily disable during build
3. **Parallel Builds** - Enable in Tools → Options → Projects and Solutions → Build and Run
4. **Incremental Linking** - Enabled by default in Debug builds

### Clean Build When Needed

Rebuild from scratch if:
- Switching between Debug/Release
- Changing platform toolset
- After updating dependencies
- Experiencing strange linker errors

---

## Getting Help

If you're still having issues:

1. **Check the logs** - Build output window shows detailed errors
2. **Search existing issues** - Someone may have had the same problem
3. **Open an issue** - Provide full error message and build log
4. **Ask in Discussions** - Community may have solutions

---

**Last Updated**: March 10, 2026  
**Status**: Most build issues resolved
