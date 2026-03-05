/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/feature/SpikeFeature.h"

//#include "world/entity/boss/enderdragon/EnderCrystal.h"
//#include "world/entity/EntityFactory.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/Level.h"
#include "world/level/LevelConstants.h"
#include "util/Random.h"

#include "world/level/levelgen/v1/TheEndRandomLevelSource.h"

SpikeFeature::EndSpike::EndSpike(int centerX, int centerZ, int radius, int height, bool guarded)
	: mCenterX(centerX)
	, mCenterZ(centerZ)
	, mRadius(radius)
	, mHeight(height)
	, mGuarded(guarded)
	, mTopBoundingBox(BlockPos(centerX - radius, 0, centerZ - radius), BlockPos(centerX + radius, TheEndGen::GEN_DEPTH, centerZ + radius)) {	
}

bool SpikeFeature::EndSpike::startsInChunk(const BlockPos& chunkOrigin) const {
	int minX = mCenterX - mRadius;
	int minZ = mCenterZ - mRadius;

	// Bitwise operation: "floor" position to chunk's origin considering it is a multiple of 16
	return chunkOrigin.x == (minX & ~0xf) && chunkOrigin.z == (minZ & ~0xf);
}

int SpikeFeature::EndSpike::getCenterX() const {
	return mCenterX;
}

int SpikeFeature::EndSpike::getCenterZ() const {
	return mCenterZ;
}

int SpikeFeature::EndSpike::getRadius() const {
	return mRadius;
}

int SpikeFeature::EndSpike::getHeight() const {
	return mHeight;
}

bool SpikeFeature::EndSpike::isGuarded() const {
	return mGuarded;
}

AABB SpikeFeature::EndSpike::getTopBoundingBox() const {
	return mTopBoundingBox;
}

/////////////////////////////////////////////////////////////////////////////////////////

SpikeFeature::SpikeFeature(const EndSpike& spike)
	: mSpike(spike)
	, mCrystalInvulnerable(false) {
}

void SpikeFeature::setCrystalInvulnerable(bool invulnerable) {
	mCrystalInvulnerable = invulnerable;
}

void SpikeFeature::setCrystalBeamTarget(const BlockPos& target) {
	mCrystalBeamTarget = target;
}

bool SpikeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	int radius = mSpike.getRadius();
	BlockPos from(pos.x - radius, 0, pos.z - radius);
	BlockPos to(pos.x + radius, mSpike.getHeight() + 10, pos.z + radius);

	for (auto& currentPos : BlockPosIterator(from, to)) {
		if ((currentPos.distSqr(BlockPos(pos.x, currentPos.y, pos.z)) <= radius * radius + 1) && (currentPos.y < mSpike.getHeight())) {
			_placeBlock(region, currentPos, FullBlock(Block::mObsidian->mID));
		}
		else if (currentPos.y > 65) {
			_placeBlock(region, currentPos, FullBlock::AIR);
		}
	}

	if (mSpike.isGuarded()) {
		for (int xx = -2; xx <= 2; xx++) {
			for (int zz = -2; zz <= 2; zz++) {
				BlockPos placePos(pos.x + xx, mSpike.getHeight(), pos.z + zz);
				if (std::abs(xx) == 2 || std::abs(zz) == 2) {
					_placeBlock(region, placePos, FullBlock(Block::mIronFence->mID));
					_placeBlock(region, placePos.above(), FullBlock(Block::mIronFence->mID));
					_placeBlock(region, placePos.above(2), FullBlock(Block::mIronFence->mID));
				}
				_placeBlock(region, placePos.above(3), FullBlock(Block::mIronFence->mID));
			}
		}
	}

	return true;
}

bool SpikeFeature::placeManually(BlockSource& region, const BlockPos& pos, Random& random, Entity* placer) {
	_setManuallyPlaced(placer);
	bool ret = place(region, pos, random);
	_setManuallyPlaced(nullptr);
	return ret;
}

void SpikeFeature::postProcessMobsAt(BlockSource& region, const BlockPos& pos, Random& random) const {
	// TODO: rherlitz
	//Unique<Entity> newEntity= EntityFactory::createSpawnedEntity(
	//	EntityType::EnderCrystal, 
	//	nullptr, 
	//	Vec3((float)pos.x + 0.5f, (float)mSpike.getHeight() + 1.0f, (float)pos.z + 0.5f),
	//	Vec2(0.0f, random.nextFloat() * 360.0f));
	//newEntity->setStatusFlag(EntityFlags::SHOW_BOTTOM, true);
	//newEntity->mInvulnerable = mCrystalInvulnerable;
	//newEntity->setBlockTarget(mCrystalBeamTarget);
	//region.getLevel().addGlobalEntity(region, std::move(newEntity));

	_placeBlock(region, BlockPos(pos.x, mSpike.getHeight(), pos.z), FullBlock(Block::mBedrock->mID, 1));
}
