/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class BookshelfBlock : public Block {
public:
	BookshelfBlock(const std::string& nameId, int id);

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
};
