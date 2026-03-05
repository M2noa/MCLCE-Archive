/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "world/level/block/Block.h"

// A beautiful flower
class ChorusFlowerBlock : public Block {
public:
	static const DataID DEAD_AGE;
	static const int GROW_RATE;
	static const int BRANCH_DIRECTIONS;

	ChorusFlowerBlock(const std::string& nameId, int id);

	virtual int getVariant(int data) const override;

	virtual void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	virtual bool isWaterBlocking() const override;

	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual void playerDestroy(Player* player, const BlockPos& pos, int data) const override;

	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	virtual bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	// If this block can provide support for another block (torch, rail, etc)
	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;

	// Functionality for instantly generating a random Chorus Plant
	static void generatePlant(BlockSource& region, const BlockPos& target, Random& random, int maxHorizontalSpread);

private:
	void _placeGrownFlower(BlockSource& region, const BlockPos& pos, DataID newAge) const;
	void _placeDeadFlower(BlockSource& region, const BlockPos& pos) const;

	static bool _allNeighborsEmpty(BlockSource& region, const BlockPos& pos, FacingID ignore);
	static void _growTreeRecursive(BlockSource& region, const BlockPos& current, const BlockPos& startPos, Random& random, int maxHorizontalSpread, int depth);
};
