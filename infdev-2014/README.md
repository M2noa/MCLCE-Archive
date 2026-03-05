# Minecraft Infdev (2014)

PC Minecraft Infdev version source code.

## source/
**Source:** `Infdev_20140630_master.zip` (gofile.io/d/RONXEc)  
**Date:** June 30, 2014  
**Platform:** PC  
**Description:** Infdev version of Minecraft from June 2014. This appears to be a custom game engine implementation.

### Structure
- `src/` - C++ source code
  - `Animation/` - Animation system
  - `Audio/` - Audio system
  - `Client/` - Client-side code
  - `Culling/` - Rendering culling system
  - `General/` - Core utilities (Vector, Matrix, Color, etc.)
  - `Input/` - Input handling
  - `Math/` - Math utilities
  - `Memory/` - Memory management and spatial partitioning (Octree)
  - `Model/` - 3D model loading (Assimp)
  - `Network/` - Networking
  - `Physics/` - Physics engine
  - `Rendering/` - OpenGL/DirectX rendering
  - `Server/` - Server-side code
  - `Utils/` - Utilities
  - `Window/` - Window management
  - `World/` - World/level management (BSP, entities)
- `vendor/` - Third-party libraries
- `CMakeLists.txt` - CMake build configuration

### Build System
Uses CMake for cross-platform building. Supports OpenGL and DirectX rendering backends.
