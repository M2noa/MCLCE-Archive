/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"
#include "util/Random.h"

class Player;
class BlockSource;

class BrewingStandBlock : public EntityBlock {
public:
	BrewingStandBlock(const std::string& nameId, int id);

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	std::string buildDescriptionName(DataID data) const override;
	void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const override;
	bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const override;

	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	bool isContainerBlock() const override;
	bool isCraftingBlock() const override;

	bool hasComparatorSignal() const override;
	int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const override;

protected:
	virtual bool canBeSilkTouched() const override;

private:
	static bool mNoDrop;
};
