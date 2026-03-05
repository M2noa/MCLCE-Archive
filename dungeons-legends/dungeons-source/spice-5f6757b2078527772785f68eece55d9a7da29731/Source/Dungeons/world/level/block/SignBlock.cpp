/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SignBlock.h"
#include "world/level/material/Material.h"
#include "world/item/Item.h"
#include "world/level/BlockSource.h"
#include "world/item/ItemInstance.h"

SignBlock::SignBlock(const std::string& nameId, int id, bool onGround)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Wood))
	, mOnGround(onGround) {
	float r = 4 / 16.0f;
	float h = 16 / 16.0f;
	setVisualShape(Vec3(0.5f - r, 0, 0.5f - r), Vec3(0.5f + r, h, 0.5f + r));
	setSolid(false);
	mProperties = BlockProperty::Sign | BlockProperty::BreakOnPush;
	mBlockEntityType = BlockEntityType::Sign;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& SignBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	getShape(getFace(region.getData(pos)), bufferAABB);
	return bufferAABB;
}

void SignBlock::getShape(int face, AABB& bufferValue) const
{
	float h0 = (4 + 0.5f) / 16.0f;
	float h1 = (12 + 0.5f) / 16.0f;
	float w0 = 0 / 16.0f;
	float w1 = 16 / 16.0f;

	float d0 = 2 / 16.0f;

	bufferValue.set(0, 0, 0, 1, 1, 1);
	if (face == 2) {
		bufferValue.set(w0, h0, 1 - d0, w1, h1, 1);
	}
	if (face == 3) {
		bufferValue.set(w0, h0, 0, w1, h1, d0);
	}
	if (face == 4) {
		bufferValue.set(1 - d0, h0, w0, 1, h1, w1);
	}
	if (face == 5) {
		bufferValue.set(0, h0, w0, d0, h1, w1);
	}
}

int SignBlock::getFace(const DataID& data) const {
	return getBlockState(BlockState::FacingDirection).get<int>(data);
}

const AABB& SignBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if(isClipping) {
		int face = getFace(region.getData(pos));
		if(mOnGround) {
			bufferValue.set(Vec3::ZERO, Vec3::ONE);
		} else {
			getShape(face, bufferValue);
		}

		return bufferValue.move(Vec3(pos));
	}

	return AABB::EMPTY;
}

bool SignBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return true;
}

int SignBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mSign->getId();
}

ItemInstance SignBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mSign);
}

void SignBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	EntityBlock::neighborChanged(region, pos, neighborPos);

	bool remove = false;

	if(mOnGround) {
		if(!region.getMaterial(pos.below()).isSolid()) {
			remove = true;
		}
	} else {
		FacingID face = getFace(region.getData(pos));
		remove = true;
		if(face == 2 && region.getMaterial(pos.south()).isSolid()) {
			remove = false;
		}
		if(face == 3 && region.getMaterial(pos.north()).isSolid()) {
			remove = false;
		}
		if(face == 4 && region.getMaterial(pos.east()).isSolid()) {
			remove = false;
		}
		if(face == 5 && region.getMaterial(pos.west()).isSolid()) {
			remove = false;
		}
	}

	if(remove) {
		spawnResources(region, pos, region.getData(pos), 1);
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
	}
}

bool SignBlock::canBeSilkTouched() const {
	return false;
}
