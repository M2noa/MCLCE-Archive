/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/biome/Biome.h"

class TheEndBiome : public Biome {
public:
	TheEndBiome(int id);
	Color getSkyColor(float temp) override;
};
