/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class HellSpringFeature : public Feature {
public:

	HellSpringFeature(BlockID block, bool insideRock);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	BlockID mBlockId;
	bool mInsideRock;
};
