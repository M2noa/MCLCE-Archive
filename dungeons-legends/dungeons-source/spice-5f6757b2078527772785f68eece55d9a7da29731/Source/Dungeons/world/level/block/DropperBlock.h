/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/DispenserBlock.h"

class DropperBlock : public DispenserBlock {
public:
	DropperBlock(const std::string& nameId, int id);

	static int getAttachedFace(int data);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	ItemInstance getSilkTouchItemInstance(DataID data) const override;

protected:
	void dispenseFrom(BlockSource &region, const BlockPos& pos) const override;

	static const int MASK_TOGGLE = 0x8;
};
