/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class BedrockBlock : public Block {
public:
	BedrockBlock(const std::string& nameId, int id);

	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const override;

	virtual bool isInfiniburnBlock(int data) const override;
};
