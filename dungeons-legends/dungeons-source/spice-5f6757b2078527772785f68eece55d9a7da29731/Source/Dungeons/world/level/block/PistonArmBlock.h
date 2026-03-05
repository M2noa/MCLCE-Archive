/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

#include "world/level/block/Block.h"
#include "world/item/Item.h"
#include "world/item/ItemInstance.h"

class PistonArmBlock : public Block {
public:
	PistonArmBlock(const std::string& nameId, int id);

	// collision
	bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const override;
	bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;

	// breaking
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	bool canBeSilkTouched() const override;

	//picking
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;
	bool getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const override;

private:
	BlockPos mPistonPos;
};