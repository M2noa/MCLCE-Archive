/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class TallGrassFeature : public Feature {
public:

	TallGrassFeature(BlockID block, DataID type);
	bool place(BlockSource& region, const BlockPos& pos, Random& random, int tries, int r) const;
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	FullBlock mGrass;
};
