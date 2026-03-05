/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/WorkbenchBlock.h"
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"
// #include "world/item/crafting/Recipe.h"
#include "world/level/Level.h"
#include "world/Facing.h"

WorkbenchBlock::WorkbenchBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Wood)) {
}


bool WorkbenchBlock::use(Player& player, const BlockPos& pos) const {
// 	player.startCrafting(pos);

	return true;
}

bool WorkbenchBlock::isCraftingBlock() const {
	return true;
}
