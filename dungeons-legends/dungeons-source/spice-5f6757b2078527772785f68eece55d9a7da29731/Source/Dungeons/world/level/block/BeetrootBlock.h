/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/CropBlock.h"

class Item;

class BeetrootBlock : public CropBlock {
public:
	BeetrootBlock(const std::string& nameId, int id);

	Item* getBaseSeed() const override;

	int getVariant(int data) const override;
	int getBasePlantId() const override;

	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;

};
