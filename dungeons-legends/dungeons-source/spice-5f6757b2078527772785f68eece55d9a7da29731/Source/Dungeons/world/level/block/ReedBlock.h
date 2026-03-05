/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "world/level/BlockSource.h"
#include "world/item/Item.h"
#include "world/item/ItemInstance.h"
#include "world/level/LevelConstants.h"

class ReedBlock : public Block {
public:

	ReedBlock(const std::string& nameId, int id);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

	int getColor(BlockSource& region, const BlockPos& pos) const override;

	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	void onGraphicsModeChanged(bool fancy_, bool preferPolyTessellation, bool transparentLeaves) override;

	virtual bool isWaterBlocking() const override {
		return true;
	}

protected:
	bool canBeSilkTouched() const override;

private:
	void checkAlive(BlockSource& region, const BlockPos& pos) const;
	int getAge(const DataID& data) const;
};
