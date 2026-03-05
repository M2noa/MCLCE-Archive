/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class FlowerFeature : public Feature {
public:

	FlowerFeature(BlockID block);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;
	bool placeFlower(BlockSource& region, const BlockPos& pos, FullBlock flower, Random& random) const;

private:

	const FullBlock mBlock;
};
