/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BushBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "LevelEvent.h"

const DataID BushBlock::MAX_GROWTH = 7;

BushBlock::BushBlock(const std::string& nameId, int id, const Material& material)
	: Block(nameId, id, material) {
	setTicking(true);
	float ss = 0.2f;
	setVisualShape( Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, ss * 3, 0.5f + ss));

	setSolid(false);
	mRenderLayer = RENDERLAYER_ALPHATEST;
	mProperties = BlockProperty::BreakOnPush;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void BushBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	checkAlive(region, pos);
}

void BushBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	checkAlive(region, pos);
}

bool BushBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	return mayPlaceOn(region.getBlock(pos.below()));
}

const AABB& BushBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		const auto& shape = getVisualShape(region, pos, bufferValue, isClipping);
		return bufferValue.set(shape.min, shape.max).move(Vec3(pos));
	}

	return AABB::EMPTY;
}

void BushBlock::growCrops(BlockSource& region, const BlockPos& pos) const {
	int growth = getBlockState(BlockState::Growth).get<int>(region.getData(pos));
	int stage = growth + region.getLevel().getRandom().nextInt(3) + 2;
	if (stage > MAX_GROWTH) {
		stage = MAX_GROWTH;
	}

	region.setBlockAndData(pos.x, pos.y, pos.z, mID, stage, Block::UPDATE_ALL);
}

bool BushBlock::mayPlaceOn(const Block& block) const {
	return block.isType(Block::mGrass) || block.isType(Block::mDirt) || block.isType(Block::mFarmland) || block.isType(Block::mPodzol);
}

void BushBlock::checkAlive(BlockSource& region, const BlockPos& pos) const {
	if (!canSurvive(region, pos)) {
		//	Only do particles when the thing is BOOM.
// 		if (mID == Block::mTallgrass->mID || mID == Block::mDoublePlant->mID || mID == Block::mDeadBush->mID) {
// 			region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos, mID);
// 		}

		spawnResources(region, pos, region.getData(pos), 1.0f);
		region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
	}
}
