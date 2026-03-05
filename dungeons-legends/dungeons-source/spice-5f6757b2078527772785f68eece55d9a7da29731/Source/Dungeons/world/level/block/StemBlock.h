/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class StemBlock : public BushBlock {
public:
	StemBlock(const std::string& nameId, int id, const Block& fruit);
	bool mayPlaceOn(const Block& block) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	int getColor(int data) const override;
	int getColor(BlockSource& region, const BlockPos& pos) const override;
	int getColor(BlockSource& region, const BlockPos& pos, DataID data) const override;

	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;
	int getConnectDir(BlockSource& region, const BlockPos& pos) const;
	
	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

protected:
	bool canBeSilkTouched() const override;

private:
	float getGrowthSpeed(BlockSource& region, const BlockPos& pos) const;
	int getGrowth(const DataID& data) const;

	const Block& mFruit;
};
