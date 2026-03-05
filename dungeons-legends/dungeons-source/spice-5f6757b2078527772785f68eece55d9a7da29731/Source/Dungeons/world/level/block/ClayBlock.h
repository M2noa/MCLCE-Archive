/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

class Random;

#include "world/level/block/Block.h"
#include "world/item/Item.h"

class ClayBlock : public Block {
public:
	ClayBlock(const std::string& nameId, int id);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
};
