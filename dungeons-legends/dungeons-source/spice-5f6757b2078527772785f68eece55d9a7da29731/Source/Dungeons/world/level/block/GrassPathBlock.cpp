/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/GrassPathBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/Facing.h"
#include "util/Random.h"

GrassPathBlock::GrassPathBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Dirt)) {

	setVisualShape( Vec3::ZERO, Vec3(1, 15 / 16.0f, 1));
	setLightBlock(Brightness::MAX);

	setSolid(false);
	mProperties = BlockProperty::Unspecified;

}

const AABB& GrassPathBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	bufferValue.set(0, 0, 0, 1, 1, 1);
	return bufferValue.move(Vec3(pos));
}


void GrassPathBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	Block::neighborChanged(region, pos, neighborPos);
	const Material& above = region.getMaterial(pos.above());
	if (above.isSolid()) {
		region.setBlock(pos, Block::mDirt->mID, Block::UPDATE_ALL);
	}
}

int GrassPathBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDirt->getResource(random, 0, bonusLootLevel);
}
