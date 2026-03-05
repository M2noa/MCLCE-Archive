/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ThinFenceBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/phys/AABB.h"
#include "world/item/ItemInstance.h"

ThinFenceBlock::ThinFenceBlock(const std::string& nameId, int id, const Material& material, bool dropsResources)
	: Block(nameId, id, material)
	, mDropsResources(dropsResources) {
	setSolid(false);
	mProperties = BlockProperty::Unspecified;
	mRenderLayer = RENDERLAYER_ALPHATEST;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int ThinFenceBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	if (!mDropsResources) {
		return 0;
	}

	return Block::getResource(random, data, bonusLootLevel);
}

bool ThinFenceBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	BlockID id = region.getBlockID(pos);
	if (id == mID) {
		return false;
	}

	return Block::shouldRenderFace(region, pos, face, shape);
}

void ThinFenceBlock::addAABBs(BlockSource& region, const BlockPos& blockPos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	bool n = attachsTo(region.getBlockID(blockPos.north()));
	bool s = attachsTo(region.getBlockID(blockPos.south()));
	bool w = attachsTo(region.getBlockID(blockPos.west()));
	bool e = attachsTo(region.getBlockID(blockPos.east()));
	AABB shape;
	Vec3 pos(blockPos);

	if ((w && e) || (!w && !e && !n && !s)) {
		shape.set(0, 0, 7.0f / 16.0f, 1, 1, 9.0f / 16.0f);
		addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);
	} else if (w && !e) {
		shape.set(0, 0, 7.0f / 16.0f, .5f, 1, 9.0f / 16.0f);
		addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);
	} else if (!w && e) {
		shape.set(.5f, 0, 7.0f / 16.0f, 1, 1, 9.0f / 16.0f);
		addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);
	}

	if ((n && s) || (!w && !e && !n && !s)) {
		shape.set(7.0f / 16.0f, 0, 0, 9.0f / 16.0f, 1, 1);
		addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);
	} else if (n && !s) {
		shape.set(7.0f / 16.0f, 0, 0, 9.0f / 16.0f, 1, .5f);
		addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);
	} else if (!n && s) {
		shape.set(7.0f / 16.0f, 0, .5f, 9.0f / 16.0f, 1, 1);
		addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);
	}
}

bool ThinFenceBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

const AABB& ThinFenceBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping /*= false*/) const {
	float minX = 7.0f / 16.0f;
	float maxX = 9.0f / 16.0f;
	float minZ = 7.0f / 16.0f;
	float maxZ = 9.0f / 16.0f;

	bool n = attachsTo(region.getBlockID(pos.north()));
	bool s = attachsTo(region.getBlockID(pos.south()));
	bool w = attachsTo(region.getBlockID(pos.west()));
	bool e = attachsTo(region.getBlockID(pos.east()));

	if ((w && e) || (!w && !e && !n && !s)) {
		minX = 0;
		maxX = 1;
	} else if (w && !e) {
		minX = 0;
	} else if (!w && e) {
		maxX = 1;
	}

	if ((n && s) || (!w && !e && !n && !s)) {
		minZ = 0;
		maxZ = 1;
	} else if (n && !s) {
		minZ = 0;
	} else if (!n && s) {
		maxZ = 1;
	}

	bufferAABB.set(minX, 0, minZ, maxX, 1, maxZ);
	return bufferAABB;
}

bool ThinFenceBlock::attachsTo(int block) const {
	return Block::mSolid[block] || block == Block::mIronFence->mID || block == Block::mGlassPane->mID || block == Block::mGlass->mID || block == Block::mStainedGlass->mID || block == Block::mStainedGlassPane->mID;
}

std::string ThinFenceBlock::buildDescriptionName(DataID data) const {
	// TEMPORARY HACK UNTIL STAINED GLASS COMES IN
	if (mID == mStainedGlassPane->mID) {
		return mGlassPane->buildDescriptionName(data);
	}
	return Block::buildDescriptionName(data);
}

ItemInstance ThinFenceBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	// TEMPORARY HACK UNTIL STAINED GLASS COMES IN
	if (mID == mStainedGlassPane->mID) {
		return ItemInstance(mGlassPane);
	}
	return Block::asItemInstance(region, pos, blockData);
}

ItemInstance ThinFenceBlock::getSilkTouchItemInstance(DataID data) const {
	// TEMPORARY HACK UNTIL STAINED GLASS COMES IN
	if (mID == mStainedGlassPane->mID) {
		return ItemInstance(mGlassPane);
	}
	return Block::getSilkTouchItemInstance(data);
}
