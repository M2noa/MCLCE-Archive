/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/StonecutterBlock.h"
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"

StonecutterBlock::StonecutterBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone)) {
}

bool StonecutterBlock::use(Player& player, const BlockPos& pos) const {
// 	if (!player.mAbilities.mInstabuild) {//TODO remove when creative inventories are added
// 		player.startStonecutting(pos);
// 	}

	return true;
}

bool StonecutterBlock::isCraftingBlock() const {
	return true;
}
