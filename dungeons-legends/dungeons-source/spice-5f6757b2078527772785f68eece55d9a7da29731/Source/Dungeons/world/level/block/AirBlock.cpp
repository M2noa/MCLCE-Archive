/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/AirBlock.h"

AirBlock::AirBlock(const std::string& nameId, int id, const Material& material)
	: Block(nameId, id, material) {
	setSolid(false);
	mThickness = 0.0f;
	mRenderLayer = RENDERLAYER_OPAQUE;
	mCanBuildOver = true;

	mProperties = BlockProperty::Unspecified;
	mCanSlide = false;
	mCanInstatick = false;

	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
	mExplosionResistance = 0.0f;
}

const AABB& AirBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	return AABB::EMPTY;
}
const AABB& AirBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	return AABB::EMPTY;
}

bool AirBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	return false;
}

bool AirBlock::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	return false;
}

bool AirBlock::isObstructingChests(BlockSource& region, const BlockPos& pos) const {
	return false;
}

bool AirBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	return false;
}

const AABB& AirBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	return AABB::EMPTY;
}

void AirBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
}

bool AirBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return true;
}

bool AirBlock::mayPick() const {
	return false;
}

bool AirBlock::mayPick(BlockSource& region, int data, bool liquid) const {
	return false;
}

bool AirBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	return false;
}

bool AirBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return false;
}

bool AirBlock::tryToPlace(BlockSource& region, const BlockPos& pos, DataID data) const {
	return false;
}

void AirBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
}

void AirBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
}

void AirBlock::destroy(BlockSource& region, const BlockPos& pos, int data, Entity* entitySource) const {
}

bool AirBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	return false;
}

void AirBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
}

bool AirBlock::canHaveExtraData() const {
	return false;
}

bool AirBlock::canBeSilkTouched() const {
	return false;
}
