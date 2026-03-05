/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class LakeFeature : public Feature {
public:

	LakeFeature(BlockID block);
	LakeFeature(BlockID block, BlockID emptyBlock);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	bool _check(const BlockPos& pos, bool* grid) const;

	BlockID mBlock;
	BlockID mEmptyBlock;
};
