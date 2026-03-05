/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ReedBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/item/ItemInstance.h"
#include "world/item/Item.h"

ReedBlock::ReedBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Plant)) {
	float ss = 6 / 16.0f;
	setVisualShape(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, 1, 0.5f + ss));
	setTicking(true);
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void ReedBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (region.isEmptyBlock(pos.above())) {
		int height = 1;

		while (region.getBlockID(pos.below(height)) == mID) {
			height++;
		}

		if (height < 3) {
			int age = getAge(region.getData(pos));
			if (age == 15) {
				region.setBlock(pos.x, pos.y + 1, pos.z, mID, Block::UPDATE_ALL);
				region.setBlockAndData(pos.x, pos.y, pos.z, mID, Block::UPDATE_NONE);
			} else {
				DataID data = 0;
				getBlockState(BlockState::Age).set(data, age + 1);
				region.setBlockAndData(pos.x, pos.y, pos.z, mID, data, Block::UPDATE_NONE);
			}
		}
	}
}

bool ReedBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	BlockID below = region.getBlockID(pos.below());
	const Material& waterMaterial = Material::getMaterial(MaterialType::Water);

	if (below == mID) {
		return true;
	}
	if (below != ((const Block*)Block::mGrass)->mID && below != Block::mDirt->mID && below != Block::mSand->mID) {
		return false;
	}
	if (region.getMaterial(pos.offset(-1, -1, 0)) == waterMaterial) {
		return true;
	}
	if (region.getMaterial(pos.offset(1, -1, 0)) == waterMaterial) {
		return true;
	}
	if (region.getMaterial(pos.offset(0, -1, -1)) == waterMaterial) {
		return true;
	}
	if (region.getMaterial(pos.offset(0, -1, 1)) == waterMaterial) {
		return true;
	}

	return false;
}

void ReedBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	checkAlive(region, pos);
}

bool ReedBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	//find the base
	int baseY = pos.y - 1, topY = pos.y + 1;

	//find top and bottom
	for (; baseY > baseY - 3 && baseY > 0 && region.getBlockID(pos.x, baseY, pos.z) == mID; --baseY) {
	}

	for (; topY < topY + 3 && topY < region.getMaxHeight() && region.getBlockID(pos.x, topY, pos.z) == mID; ++topY) {
	}

	++baseY;//correct last failed iteration
	--topY;

	int toPlace = 3 - (topY - baseY) - 1;
	if (toPlace <= 0) {
		return false;	//too high
	} else {
		for (auto i : range_incl(topY + 1, topY + toPlace)) {
			if (!region.isEmptyBlock(pos.x, i, pos.z)) {
				break;
			}

			region.setBlockAndData(pos.x, i, pos.z, mID, Block::UPDATE_ALL);
		}

		return true;
	}
}

int ReedBlock::getColor(BlockSource& region, const BlockPos& pos) const {
	return region.getGrassColor(pos);
}

bool ReedBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	return mayPlace(region, pos);
}

const AABB& ReedBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		return bufferValue.set(Vec3::ZERO, Vec3::ONE).move(Vec3(pos));
	}
	return AABB::EMPTY;
}

int ReedBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mReeds->getId();
}

ItemInstance ReedBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mReeds);
}

void ReedBlock::onGraphicsModeChanged(bool fancy_, bool preferPolyTessellation, bool transparentLeaves) {
	Block::onGraphicsModeChanged(fancy_, preferPolyTessellation, transparentLeaves);

	mRenderLayer = mFancy ? RENDERLAYER_OPTIONAL_ALPHATEST : RENDERLAYER_ALPHATEST;
}

bool ReedBlock::canBeSilkTouched() const {
	return false;
}

void ReedBlock::checkAlive(BlockSource& region, const BlockPos& pos) const {
	if (!canSurvive(region, pos)) {
		popResource(region, pos, ItemInstance(Item::mReeds));
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
	}
}

int ReedBlock::getAge(const DataID& data) const {
	return getBlockState(BlockState::Age).get<int>(data);
}
