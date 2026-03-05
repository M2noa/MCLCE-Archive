/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class LadderBlock : public Block {
public:
	LadderBlock(const std::string& nameId, int id);

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	virtual const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;
	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual bool isWaterBlocking() const override {
		return true;
	}

protected:
	virtual bool canBeSilkTouched() const override;
};
