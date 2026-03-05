/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/WoolCarpetBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/Facing.h"

WoolCarpetBlock::WoolCarpetBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Carpet)) {
	setVisualShape(Vec3::ZERO, Vec3(1, 1 / 16.0f, 1));
	setTicking(true);
	setSolid(false);
	mThickness = 0.07f;

	mTranslucency[id] = 0.89f;
	mProperties = BlockProperty::Carpet | BlockProperty::Walkable;
}

const AABB& WoolCarpetBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	int height = 0;
	float o = 1 * (1 + height) / 16.0f;
	bufferAABB.set(Vec3::ZERO, Vec3(1, o, 1));
	return bufferAABB;
}

bool WoolCarpetBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return Block::mayPlace(region, pos) && canSurvive(region, pos);
}

void WoolCarpetBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	if (canSurvive(region, pos)) {
		Block::onPlace(region, pos);
	}
}

void WoolCarpetBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	canSurvive(region, pos);
}

bool WoolCarpetBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	if(region.isEmptyBlock(pos.below())) {
		if (region.getBlockID(pos) == mID) {
			spawnResources(region, pos, region.getData(pos), 1.0f);
			region.removeBlock(pos.x, pos.y, pos.z);
		}

		return false;
	}

	return true;
}

bool WoolCarpetBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	if(face == Facing::UP) {
		return true;
	}

	return Block::shouldRenderFace(region, pos, face, shape);
}

DataID WoolCarpetBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}


Color WoolCarpetBlock::getMapColor(BlockSource& region, const BlockPos& pos) const {
	return Block::mWool->getMapColor(region, pos);
}

bool WoolCarpetBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return true;
}

int WoolCarpetBlock::getBlockDataForItemAuxValue(int auxValue) {
	return ~auxValue & 0xf;
}
