/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "SoulSandBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/entity/Entity.h"
#include "world/phys/AABB.h"

SoulSandBlock::SoulSandBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Sand)) {
}

const AABB& SoulSandBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	float r = 2 / 16.0f;
	bufferValue.set((float)pos.x, (float)pos.y, (float)pos.z, (float)pos.x + 1, (float)pos.y + 1 - r, (float)pos.z + 1);
	return bufferValue;
}

void SoulSandBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
	entity.mPosDelta.x *= 0.4f;
	entity.mPosDelta.z *= 0.4f;
}

bool SoulSandBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	// if air is above us then we can travel on this soul sand (because the top block you can move on)
	BlockID blockId = entity.getRegion().getBlockID(pathPos.above());
	if (blockId == BlockID::AIR) {
		return true;
	}

	return !mMaterial.getBlocksMotion();
}
