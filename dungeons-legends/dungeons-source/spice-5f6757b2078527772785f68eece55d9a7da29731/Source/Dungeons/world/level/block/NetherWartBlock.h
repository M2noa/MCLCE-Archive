/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class NetherWartBlock : public BushBlock {
public:
	NetherWartBlock(const std::string& nameId, int id);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;

	void growCropsToMax(BlockSource& region, const BlockPos& pos);

protected:
	bool canBeSilkTouched() const override;

	bool mayPlaceOn(const Block& block) const override;

private:
	static const int MAX_AGE;
};
