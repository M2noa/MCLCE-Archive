/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/TreeFeature.h"

class GroundBushFeature : public TreeFeature {
public:

	GroundBushFeature(int trunkType, int leafType);

	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;
};
