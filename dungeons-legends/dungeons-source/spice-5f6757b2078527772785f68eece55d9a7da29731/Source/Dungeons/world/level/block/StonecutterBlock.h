/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class StonecutterBlock : public Block {
public:
	StonecutterBlock(const std::string& nameId, int id);

	bool use(Player& player, const BlockPos& pos) const override;

	bool isCraftingBlock() const override;

};
