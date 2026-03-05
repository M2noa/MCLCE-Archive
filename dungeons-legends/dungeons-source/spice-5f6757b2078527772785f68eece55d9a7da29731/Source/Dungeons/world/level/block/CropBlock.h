/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class Item;
class BlockSource;

class CropBlock : public BushBlock {
public:
	CropBlock(const std::string& nameId, int id);
	bool mayPlaceOn(const Block& block) const override;
	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	int getVariant(int data) const override;

	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	void growCropsToMax(BlockSource& region, const BlockPos& pos);

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	bool isCropBlock() const override;

protected:
	virtual Item* getBaseSeed() const;
	virtual int getBasePlantId() const;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	bool canBeSilkTouched() const override;

private:
	float getGrowthSpeed(BlockSource& region, const BlockPos& pos) const;
};
