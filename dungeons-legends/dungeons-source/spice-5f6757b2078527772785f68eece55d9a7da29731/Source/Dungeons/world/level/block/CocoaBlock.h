/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class CocoaBlock : public Block {
public:
	static int getAge(int data);

	CocoaBlock(const std::string& nameId, int id);

	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;
	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

protected:
	bool canBeSilkTouched() const override;
};
