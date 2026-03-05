/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class FarmBlock : public Block {
public:
	FarmBlock(const std::string& nameId, int id);

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	int getVariant(int data) const override;

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	void onFallOn(BlockSource& region, const BlockPos& pos, Entity* entity, float fallDistance) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

protected:
	bool canBeSilkTouched() const override;

private:
	bool isUnderCrops(BlockSource& region, const BlockPos& pos) const;
	bool isNearWater(BlockSource& region, const BlockPos& pos) const;

};
