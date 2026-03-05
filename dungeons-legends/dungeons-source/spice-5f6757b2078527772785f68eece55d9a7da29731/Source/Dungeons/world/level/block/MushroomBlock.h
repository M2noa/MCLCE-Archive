/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class MushroomBlock : public BushBlock {
public:
	MushroomBlock(const std::string& nameId, int id);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	bool mayPlaceOn(const Block& block) const override;

	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

private:
	void _growTree(BlockSource& region, const BlockPos& pos, Random& random) const;
};
