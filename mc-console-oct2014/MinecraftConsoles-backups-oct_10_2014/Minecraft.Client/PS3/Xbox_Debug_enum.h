#pragma once

enum eDebugSetting
{
	eDebugSetting_LoadSavesFromDisk,
	eDebugSetting_WriteSavesToDisk,
	eDebugSetting_InterfaceOff,
	eDebugSetting_Safearea,
	eDebugSetting_MobsDontAttack,
	eDebugSetting_FreezeTime,
	eDebugSetting_DisableWeather,
	eDebugSetting_CraftAnything,
	eDebugSetting_UseDpadForDebug,
	eDebugSetting_MobsDontTick,	
	eDebugSetting_InstantDestroy,
	eDebugSetting_HandRenderingOff,
	eDebugSetting_RemoveAllPlayerData,
	eDebugSetting_DebugLeaderboards,
	eDebugSetting_TipsAlwaysOn,
	//eDebugSetting_LightDarkBackground,
	eDebugSetting_RegularLightning,
	eDebugSetting_Max,
};

enum eDebugButton
{
	eDebugButton_Theme=0,
	eDebugButton_Avatar_Item_1,
	eDebugButton_Avatar_Item_2,
	eDebugButton_Avatar_Item_3,
	eDebugButton_Gamerpic_1,
	eDebugButton_Gamerpic_2,
	eDebugButton_CheckTips,
	eDebugButton_WipeLeaderboards,
	eDebugButton_Max,
};