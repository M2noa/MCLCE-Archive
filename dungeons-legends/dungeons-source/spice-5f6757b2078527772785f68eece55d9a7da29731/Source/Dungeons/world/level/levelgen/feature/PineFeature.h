/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/TreeFeature.h"

class PineFeature : public TreeFeature {

public:

	PineFeature(Entity* placer = nullptr);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;
};
