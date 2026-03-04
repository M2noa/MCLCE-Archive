// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#ifdef __PS3__
#else
#define AUTO_VAR(_var, _val) auto _var = _val
#endif




#if defined __PS3__
// C RunTime Header Files
#include <stdlib.h>
#endif



#if defined(__PS3__)
#include <cell/l10n.h>
#include <cell/pad.h>
#include <cell/cell_fs.h>
#include <sys/process.h>
#include <sys/ppu_thread.h>
#include <cell/sysmodule.h>
#include <sysutil/sysutil_common.h>
#include <sysutil/sysutil_savedata.h>
#include <sysutil/sysutil_sysparam.h>


#include "Ps3Types.h"
#include "Ps3Stubs.h"
#include "PS3Maths.h"

#else
#include <unordered_map>
#include <unordered_set>
#include <sal.h>
#include <vector>
#endif

#include <memory>

#include <list>
#include <map>
#include <set>
#include <queue>
#include <deque>
#include <algorithm>
#include <math.h>
#include <limits>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>

#ifndef __PS3__			// the PS3 lib assert is rubbish, and aborts the code, we define our own in PS3Types.h
#include <assert.h>
#endif

#include "extraX64.h"

#include "Definitions.h"
#include "Class.h"
#include "Exceptions.h"
#include "Mth.h"
#include "StringHelpers.h"
#include "ArrayWithLength.h"
#include "Random.h"
#include "TilePos.h"
#include "ChunkPos.h"
#include "compression.h"
#include "PerformanceTimer.h"


#ifdef _FINAL_BUILD
#define printf BREAKTHECOMPILE
#define wprintf BREAKTHECOMPILE
#undef OutputDebugString
#define OutputDebugString BREAKTHECOMPILE
#define OutputDebugStringA BREAKTHECOMPILE
#define OutputDebugStringW BREAKTHECOMPILE
#endif


void MemSect(int sect);

#if defined (__PS3__)
#include "..\Minecraft.Client\PS3\4JLibs\inc\4J_Profile.h"
#include "..\Minecraft.Client\PS3\4JLibs\inc\4J_Render.h"
#include "..\Minecraft.Client\PS3\4JLibs\inc\4J_Storage.h"
#include "..\Minecraft.Client\PS3\4JLibs\inc\4J_Input.h"
#else
#include "..\Minecraft.Client\Orbis\4JLibs\inc\4J_Profile.h"
#include "..\Minecraft.Client\Orbis\4JLibs\inc\4J_Render.h"
#include "..\Minecraft.Client\Orbis\4JLibs\inc\4J_Storage.h"
#include "..\Minecraft.Client\Orbis\4JLibs\inc\4J_Input.h"
#endif

#include "..\Minecraft.Client\Common\Network\GameNetworkManager.h"

// #ifdef _XBOX
#include "..\Minecraft.Client\Common\UI\UIEnums.h"
#include "..\Minecraft.Client\Common\App_defines.h"
#include "..\Minecraft.Client\Common\App_enums.h"
#include "..\Minecraft.Client\Common\Tutorial\TutorialEnum.h"
#include "..\Minecraft.Client\Common\App_structs.h"
//#endif

#include "..\Minecraft.Client\Common\Consoles_App.h"
#include "..\Minecraft.Client\Common\Minecraft_Macros.h"
#include "..\Minecraft.Client\Common\Colours\ColourTable.h"

#include "..\Minecraft.Client\Common\BuildVer.h"

#if defined (__PS3__)
#include "..\Minecraft.Client\PS3\PS3_App.h"
#include "..\Minecraft.Client\PS3Media\strings.h"
#include "..\Minecraft.Client\PS3\Sentient\SentientTelemetryCommon.h"
#include "..\Minecraft.Client\PS3\Sentient\MinecraftTelemetry.h"

#else
#include "..\Minecraft.Client\Orbis\Orbis_App.h"
#include "..\Minecraft.Client\OrbisMedia\strings.h"
#include "..\Minecraft.Client\Orbis\Sentient\SentientTelemetryCommon.h"
#include "..\Minecraft.Client\Orbis\Sentient\MinecraftTelemetry.h"
#endif

#include "..\Minecraft.Client\Common\DLC\DLCSkinFile.h"
#include "..\Minecraft.Client\Common\Console_Awards_enum.h"
#include "..\Minecraft.Client\Common\Potion_Macros.h"
#include "..\Minecraft.Client\Common\Console_Debug_enum.h"
#include "..\Minecraft.Client\Common\GameRules\ConsoleGameRulesConstants.h"
#include "..\Minecraft.Client\Common\GameRules\ConsoleGameRules.h"
#include "..\Minecraft.Client\Common\Telemetry\TelemetryManager.h"
