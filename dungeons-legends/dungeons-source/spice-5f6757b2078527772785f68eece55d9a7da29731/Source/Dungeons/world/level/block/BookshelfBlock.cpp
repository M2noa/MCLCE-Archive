/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BookshelfBlock.h"
#include "world/item/Item.h"
#include "world/level/material/Material.h"

BookshelfBlock::BookshelfBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Wood)) {
}

int BookshelfBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 3;
}

int BookshelfBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mBook->getId();
}
