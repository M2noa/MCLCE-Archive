/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/GravelBlock.h"
#include "world/level/material/Material.h"
#include "util/Random.h"
#include "world/item/Item.h"
#include "Core/Math/Color.h"

GravelBlock::GravelBlock(const std::string& nameId, int type)
	: HeavyBlock(nameId, type, Material::getMaterial(MaterialType::Sand)) {
}

int GravelBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	bonusLootLevel = std::min(3, bonusLootLevel);

	if (random.nextInt(10 - bonusLootLevel * 3) == 0) {
		return Item::mFlint->getId();
	}

	return mID;
}

Color GravelBlock::getDustColor(DataID data) const {
	return Color::fromARGB(0xff807c7b);
}
