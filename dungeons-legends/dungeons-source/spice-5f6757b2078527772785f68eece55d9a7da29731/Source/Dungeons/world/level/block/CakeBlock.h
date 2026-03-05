/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "world/item/Item.h"

class Player;
class BlockSource;
class Facing;

class CakeBlock : public Block {
public:
	CakeBlock(const std::string& nameId, int id);

	bool use(Player& player, const BlockPos& pos) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;

	const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;
	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

// 	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool hasComparatorSignal() const override;
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

private:
	virtual bool canBeSilkTouched() const override;

	static const int MAX_BITES = 6;
};
