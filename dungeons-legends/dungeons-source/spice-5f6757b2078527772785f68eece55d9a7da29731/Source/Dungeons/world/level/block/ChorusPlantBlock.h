/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "world/level/block/Block.h"

class ChorusPlantBlock : public Block {
public:
	static const float PLANT_WIDTH;
	static const float PLANT_HEIGHT;
	static const float PLANT_ITEM_DIMENSION;

	ChorusPlantBlock(const std::string& nameId, int id);

	virtual void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	virtual bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	virtual bool isWaterBlocking() const override;

	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	virtual bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	virtual bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;

	// Check if the neighbor block can be connected to
	bool connectsTo(BlockSource& region, const BlockPos& thisPos, const BlockPos& otherPos) const;

	bool canBeSilkTouched() const override;
	// If this block can provide support for another block (torch, rail, etc)
	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;
};
