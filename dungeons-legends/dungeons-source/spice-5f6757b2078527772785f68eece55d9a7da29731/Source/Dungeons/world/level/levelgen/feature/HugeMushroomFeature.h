/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/Feature.h"

class HugeMushroomFeature : public Feature {
public:

	HugeMushroomFeature();
	HugeMushroomFeature(int forcedType);

	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	bool _canSurvive(BlockID belowBlock) const;

	int mForcedType;
};
