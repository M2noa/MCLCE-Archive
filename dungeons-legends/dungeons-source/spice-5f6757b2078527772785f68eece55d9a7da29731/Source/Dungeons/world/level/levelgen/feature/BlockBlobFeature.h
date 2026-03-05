/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class BlockBlobFeature : public Feature {
public:

	BlockBlobFeature(BlockID block, int radius);

	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

protected:

	FullBlock mBlock;
	int mStartRadius;
};
