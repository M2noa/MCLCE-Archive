#ifndef PRIVATE_CONSTANTS_H
#define PRIVATE_CONSTANTS_H

// This file is included both in code files (cpp) and in resource files (rc).  Don't make changes here unless
// you're confident that your changes match both the cpp and rc preprocessor syntaxes (they are slightly different)

// You should not be using anything in this file unless you are changing version numbers, or editing specific files

// do not use these directly, use SharedConstants::Major, Minor, Patch, Beta
#define PRIVATE_MAJOR 1
#define PRIVATE_MINOR 0
#define PRIVATE_PATCH 2
#define PRIVATE_REVISION 0
#define IS_BETA 0

#define STR_MACRO(s) #s
#define STR_STR_MACRO(s) STR_MACRO(s)

#define PRIVATE_VERSION PRIVATE_MAJOR, PRIVATE_MINOR, PRIVATE_PATCH, PRIVATE_REVISION
#define PRIVATE_VER_STR STR_STR_MACRO(PRIVATE_MAJOR.PRIVATE_MINOR.PRIVATE_PATCH.PRIVATE_REVISION)

#if defined(MCPE_PLATFORM_WIN32)
#define PRIVATE_APP_NAME "Minecraft Desktop"
#elif defined(MCPE_PLATFORM_WINSTORE)
#if defined(MCPE_PLATFORM_HOLOLENS)
#define PRIVATE_APP_NAME "Minecraft: Hololens"
#else
#define PRIVATE_APP_NAME "Minecraft: Windows 10 Edition"
#endif
#else
#define PRIVATE_APP_NAME "UNKNOWN"
#endif

#endif
