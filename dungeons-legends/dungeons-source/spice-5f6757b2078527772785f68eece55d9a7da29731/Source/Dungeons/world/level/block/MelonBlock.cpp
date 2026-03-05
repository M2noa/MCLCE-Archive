/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/MelonBlock.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/item/Item.h"

MelonBlock::MelonBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Vegetable)){
	mProperties = BlockProperty::CubeShaped | BlockProperty::BreakOnPush;
}

int MelonBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mMelon->getId();
}

int MelonBlock::getResourceCount( Random& random, int data, int bonusLootLevel) const {
	return std::min(9, 3 + random.nextInt(5 + bonusLootLevel));
}
