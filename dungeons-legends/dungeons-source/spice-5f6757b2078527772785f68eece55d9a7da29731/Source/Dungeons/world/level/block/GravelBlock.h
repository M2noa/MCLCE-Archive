/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/HeavyBlock.h"
#include "world/item/Item.h"

class GravelBlock : public HeavyBlock {
public:
	GravelBlock(const std::string& nameId, int type);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;

	Color getDustColor(DataID data) const override;
};
