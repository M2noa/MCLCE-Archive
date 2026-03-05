/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class OreFeature : public Feature {

public:

	OreFeature(BlockID block, int count);
	OreFeature(BlockID block, DataID blockData, int count);

	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	static bool _isDiggable(BlockID block);

	FullBlock mBlock;
	int mCount;
};
