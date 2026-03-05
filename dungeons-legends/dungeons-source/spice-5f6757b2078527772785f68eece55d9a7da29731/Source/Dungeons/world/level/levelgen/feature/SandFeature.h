/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class SandFeature : public Feature {
public:

	SandFeature(BlockID block, int radius);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	FullBlock mBlock;
	int mRadius;
};
