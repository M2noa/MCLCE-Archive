/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SnowBlock.h"
#include "world/level/block/TopSnowBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
//#include "world/level/LightLayer.h"
#include "world/item/Item.h"

SnowBlock::SnowBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Snow)) {
	mProperties = mProperties | BlockProperty::Snow | BlockProperty::TopSolidBlocking;
	mBrightnessGamma = 0.45f;
}

int SnowBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mSnowBall->getId();
}

int SnowBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 4;
}

bool SnowBlock::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	// Collision Shape
	outAABB.set((float)pos.x, (float)pos.y, (float)pos.z, (float)pos.x + 1.0f, (float)pos.y + 1.0f, (float)pos.z + 1.0f);
	return true;
}

float SnowBlock::calcGroundFriction(Mob& mob, BlockPos const& blockPos) const {
	return 0.6f;// regular ground friction
}
