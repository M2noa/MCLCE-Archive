/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "ClayBlock.h"
#include "world/level/material/Material.h"
#include "world/item/Item.h"

ClayBlock::ClayBlock(const std::string& nameId, int id) 
	: Block(nameId, id, Material::getMaterial(MaterialType::Clay)) {
}

int ClayBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mClay->getId();
}

int ClayBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 4;
}
