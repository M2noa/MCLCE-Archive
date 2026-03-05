/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once


#include "world/level/levelgen/feature/Feature.h"

class EndPodiumFeature : public Feature {
public:
	static int PODIUM_RADIUS;
	static int PODIUM_PILLAR_HEIGHT;
	static int RIM_RADIUS;
	static float CORNER_ROUNDING;
	static BlockPos END_PODIUM_LOCATION;
	static BlockPos END_PODIUM_CHUNK_POSITION;

private:
	bool mActive;

public:
	EndPodiumFeature(bool active);
	bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;
};

