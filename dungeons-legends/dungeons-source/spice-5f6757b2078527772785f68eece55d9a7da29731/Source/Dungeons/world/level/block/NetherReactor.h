/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class Material;
class Item;

class NetherReactorBlock : public EntityBlock {
public:
	NetherReactorBlock(const std::string& nameId, int id, const Material& material);

	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;

private:
	void _dropItem(BlockSource& region, const BlockPos& pos, Item* item) const;
};
