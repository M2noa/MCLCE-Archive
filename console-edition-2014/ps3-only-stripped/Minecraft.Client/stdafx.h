// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#include <xtl.h>
//#include <xboxmath.h>

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : 4J Warning Msg: "

// use  - #pragma message(__LOC__"Need to do something here")

// #ifndef _XBOX
// #ifdef _CONTENT_PACKAGE
// #define TO_BE_IMPLEMENTED
// #endif
// #endif

#if defined(__PS3__)

#include "Ps3Types.h"
#include "Ps3Stubs.h"
#include "PS3Maths.h"

#else
#define AUTO_VAR(_var, _val) auto _var = _val
#include <unordered_map>
#include <unordered_set>
#include <vector>
typedef unsigned __int64 __uint64;
#endif







#include "extraX64.h"

#ifdef __PS3__
#include <cell/rtc.h>
#include <cell/l10n.h>
#include <cell/pad.h>
#include <cell/cell_fs.h>
#include <sys/process.h>
#include <sys/ppu_thread.h>
#include <cell/sysmodule.h>
#include <sysutil/sysutil_common.h>
#include <sysutil/sysutil_savedata.h>
#include <sysutil/sysutil_sysparam.h>

#endif

// C RunTime Header Files
#include <stdlib.h>

#include <memory>

#include <list>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>

#include <assert.h>


#include "..\Minecraft.World\Definitions.h"
#include "..\Minecraft.World\class.h"
#include "..\Minecraft.World\ArrayWithLength.h"
#include "..\Minecraft.World\SharedConstants.h"
#include "..\Minecraft.World\Random.h"
#include "..\Minecraft.World\compression.h"
#include "..\Minecraft.World\PerformanceTimer.h"

#if defined (__PS3__)

	#include "PS3\4JLibs\inc\4J_Input.h"
	#include "PS3\4JLibs\inc\4J_Profile.h"
	#include "PS3\4JLibs\inc\4J_Render.h"
	#include "PS3\4JLibs\inc\4J_Storage.h"
#else
	#include "Orbis\4JLibs\inc\4J_Input.h"	
	#include "Orbis\4JLibs\inc\4J_Profile.h"
	#include "Orbis\4JLibs\inc\4J_Render.h"
	#include "Orbis\4JLibs\inc\4J_Storage.h"
#endif

#include "Textures.h"
#include "Font.h"
#include "ClientConstants.h"
#include "Gui.h"
#include "Screen.h"
#include "ScreenSizeCalculator.h"
#include "Minecraft.h"
#include "MemoryTracker.h"
#include "stubs.h"
#include "BufferedImage.h"

#include "Common\Network\GameNetworkManager.h"


#include "Common\UI\UIEnums.h"
#include "Common\UI\UIStructs.h"
// #ifdef _XBOX
#include "Common\App_defines.h"
#include "Common\App_enums.h"
#include "Common\Tutorial\TutorialEnum.h"
#include "Common\App_structs.h"
//#endif

#include "Common\Consoles_App.h"
#include "Common\Minecraft_Macros.h"
#include "Common\BuildVer.h"

#if defined (__PS3__)
	#include "extraX64client.h"
	#include "PS3\Sentient\MinecraftTelemetry.h"
	#include "PS3\Sentient\DynamicConfigurations.h"
	#include "PS3\Sentient\SentientTelemetryCommon.h"
	#include "PS3Media\strings.h"
	#include "PS3\PS3_App.h"
	#include "PS3\GameConfig\Minecraft.spa.h"
	#include "PS3Media\4J_strings.h"
	#include "PS3\XML\ATGXmlParser.h"
	#include "PS3\Social\SocialManager.h"
	#include "Common\Audio\SoundEngine.h"
	#include "PS3\Iggy\include\iggy.h"
	#include "PS3\Iggy\gdraw\gdraw_ps3gcm.h"
	#include "PS3\PS3_UIController.h"
#else
	#include "Orbis\Sentient\MinecraftTelemetry.h"
	#include "OrbisMedia\strings.h"
	#include "Orbis\Orbis_App.h"
	#include "Orbis\Sentient\SentientTelemetryCommon.h"
	#include "Orbis\Sentient\DynamicConfigurations.h"
	#include "Orbis\GameConfig\Minecraft.spa.h"
	#include "OrbisMedia\4J_strings.h"
	#include "Orbis\XML\ATGXmlParser.h"	
	#include "Windows64\Social\SocialManager.h"
	#include "Common\Audio\SoundEngine.h"
	#include "Orbis\Iggy\include\iggy.h"
	#include "Orbis\Iggy\gdraw\gdraw_orbis.h"
	#include "Orbis\Orbis_UIController.h"
#endif

#include "Common\ConsoleGameMode.h"
#include "Common\Console_Debug_enum.h"
#include "Common\Console_Awards_enum.h"
#include "Common\Tutorial\TutorialMode.h"
#include "Common\Tutorial\Tutorial.h"
#include "Common\Tutorial\FullTutorialMode.h"
#include "Common\Trial\TrialMode.h"
#include "Common\GameRules\ConsoleGameRules.h"
#include "Common\GameRules\ConsoleSchematicFile.h"
#include "Common\Colours\ColourTable.h"
#include "Common\DLC\DLCSkinFile.h"
#include "Common\DLC\DLCManager.h"
#include "Common\DLC\DLCPack.h"
#include "Common\Telemetry\TelemetryManager.h"

#if !defined(__PS3__)
#include "extraX64client.h"
#endif



#ifdef _FINAL_BUILD
#define printf BREAKTHECOMPILE
#define wprintf BREAKTHECOMPILE
#undef OutputDebugString
#define OutputDebugString BREAKTHECOMPILE
#define OutputDebugStringA BREAKTHECOMPILE
#define OutputDebugStringW BREAKTHECOMPILE
#endif

void MemSect(int sect);
