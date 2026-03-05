/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "util/EnumHash.h"

enum class GameType : int {
	Undefined = -1,
	Survival = 0,
	Creative = 1,
	//Holographic gameplay modes are enumerated here, alongside their classic counterparts
	SurvivalViewer = 2,	//Holographic -- reverts to "survival", some abilities may be limited
	CreativeViewer = 3,	//Holographic -- same as SurvivalViewer, but with full ability and reverts to "creative"

	Default = Survival
};
ENUM_HASH(GameType)
