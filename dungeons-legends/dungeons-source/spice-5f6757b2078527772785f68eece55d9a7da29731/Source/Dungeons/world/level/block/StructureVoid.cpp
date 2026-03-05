/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/StructureVoid.h"
#include "world/level/material/Material.h"

StructureVoid::StructureVoid(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Slime)) {
	setSolid(false);
	mRenderLayer = RENDERLAYER_BLEND;
	mCanSlide = false;

	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());

	setVisualShape(Vec3(5.0f / 16.0f), Vec3(11.0f / 16.0f));
}

bool StructureVoid::isObstructingChests(BlockSource& region, const BlockPos& pos) const {
	return false;
}

bool StructureVoid::isWaterBlocking() const {
	return false;
}

bool StructureVoid::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	return false;
}

bool StructureVoid::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	return false;
}

bool StructureVoid::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return true;
}

void StructureVoid::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
}

bool StructureVoid::canHaveExtraData() const {
	return false;
}

bool StructureVoid::canBeSilkTouched() const {
	return false;
}
