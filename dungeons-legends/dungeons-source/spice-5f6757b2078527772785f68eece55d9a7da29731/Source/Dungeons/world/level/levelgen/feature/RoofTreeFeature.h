/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/TreeFeature.h"
#include "world/level/LevelConstants.h"

class RoofTreeFeature : public TreeFeature {
public:

	RoofTreeFeature(Entity* placer);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	void _placeVines(BlockSource& region, const BlockPos& pos, Random& random, bool shouldPlace, int dirx, int dirz) const;
	void _placeVine(BlockSource& region, const BlockPos& pos, Random& random, int direction) const;
	void _placeLeafs(BlockSource& region, const BlockPos& leafPos, const BlockPos& pos, Random& random) const;
	void _placeLeafAt(BlockSource& region, const BlockPos& pos) const;
};
