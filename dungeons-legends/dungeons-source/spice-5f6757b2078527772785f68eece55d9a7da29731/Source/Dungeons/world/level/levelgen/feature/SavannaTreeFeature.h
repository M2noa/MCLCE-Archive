/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/TreeFeature.h"
#include "world/level/LevelConstants.h"
class SavannaTreeFeature : public TreeFeature {
public:

	SavannaTreeFeature(Entity* manuallyPlaced = nullptr);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	void _placeLeafAt(BlockSource& region, const BlockPos& pos) const;
};
