/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/HugeMushroomBlock.h"
#include "world/Facing.h"
#include "util/Random.h"

HugeMushroomBlock::HugeMushroomBlock(const std::string& nameId, int id, const Material& material, Type type)
	: Block(nameId, id, material)
	, mType(type) {
}

int HugeMushroomBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return mType == Type::Brown ? Block::mBrownMushroom->mID : Block::mRedMushroom->mID;
}

int HugeMushroomBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	int count = random.nextInt(10) - 7;
	if(count < 0) {
		count = 0;
	}

	return count;
}

DataID HugeMushroomBlock::getSpawnResourcesAuxValue(DataID data) const {
	return 14;	// corresponds to all outside texture
}
