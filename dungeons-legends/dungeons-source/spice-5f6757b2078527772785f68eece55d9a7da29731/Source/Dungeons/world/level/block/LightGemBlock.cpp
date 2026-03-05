/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/LightGemBlock.h"
#include "util/Random.h"
#include "world/item/Item.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/PoweredBlockComponent.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"

LightGemBlock::LightGemBlock(const std::string& nameId, int id, const Material& material )
	: Block(nameId, id, material) {
	mProperties = mProperties | BlockProperty::Power_BlockDown | BlockProperty::Power_NO;
}

int LightGemBlock::getResourceCount( Random& random, int data, int bonusLootLevel) const {
	return std::min(4, 2 + random.nextInt(3 + bonusLootLevel));
}

int LightGemBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mYellowDust->getId();
}
 