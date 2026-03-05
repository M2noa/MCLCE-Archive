/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/LeafBlock.h"

class NewLeafBlock : public LeafBlock {
public:
	static const int ACACIA_LEAF = 0;
	static const int BIG_OAK_LEAF = 1;
	static const int NUM_TYPES = 2;

	NewLeafBlock(const std::string& nameId, int id);

	int getVariant(int data) const override;

	void dropExtraLoot(BlockSource& region, const BlockPos& pos, int data) const override;

	std::string buildDescriptionName(DataID data) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;
};
