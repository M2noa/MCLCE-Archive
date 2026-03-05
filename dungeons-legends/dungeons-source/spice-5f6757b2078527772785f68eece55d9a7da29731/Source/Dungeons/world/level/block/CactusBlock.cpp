/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/CactusBlock.h"

#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/entity/Entity.h"
#include "world/phys/AABB.h"
#include "world/Facing.h"
#include "world/level/BlockSource.h"

CactusBlock::CactusBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Cactus)){

	setTicking(true);
	setSolid(false);
	setPushesOutItems(true);
	setVisualShape(Vec3(SIZE_OFFSET), Vec3::ONE - Vec3(SIZE_OFFSET));
	mProperties = BlockProperty::BreakOnPush;
}

void CactusBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const{
	if (region.isEmptyBlock(pos.above())) {
		int height = 1;

		while (region.getBlockID(pos.below(height)) == mID) {
			height++;
		}

		if (height < 3) {
			int age = getBlockState(BlockState::Age).get<int>(region.getData(pos));
			// It takes way to long on pocket edition because of fewer ticks
			if (age >= /*15*/ 10) {
				region.setBlock(pos.above(), mID, Block::UPDATE_ALL);
				region.setBlockAndData(pos, mID, Block::UPDATE_NONE);
				neighborChanged(region, pos.above(), pos.above());
			} else {
				region.setBlockAndData(pos.x, pos.y, pos.z, mID, age + 1, Block::UPDATE_NONE);
			}
		}
	}
}

const AABB& CactusBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	float r = 1 / 16.0f;
	bufferValue.set(Vec3(pos.x + r, (float)pos.y, pos.z + r), Vec3(pos.x + 1 - r, (float)(pos.y + 1 - r), pos.z + 1 - r)); // cactus occupies the full range in y axis
	return bufferValue;
}

bool CactusBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	if (!Block::mayPlace(region, pos)) {
		return false;
	}

	return canSurvive(region, pos);
}

void CactusBlock::onGraphicsModeChanged(bool fancy_, bool preferPolyTessellation, bool transparentLeaves){
	Block::onGraphicsModeChanged(fancy_, preferPolyTessellation, transparentLeaves);
	mRenderLayer = fancy_ ? RENDERLAYER_OPTIONAL_ALPHATEST : RENDERLAYER_ALPHATEST;
}

void CactusBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!canSurvive(region, pos)) {
		// Should always spawn a cactus when this happens
		spawnResources(region, pos, region.getData(pos), 1);
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
	}
}

bool CactusBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	if (region.getMaterial(pos.west()).isSolid()) {
		return false;
	}

	if (region.getMaterial(pos.east()).isSolid()) {
		return false;
	}

	if (region.getMaterial(pos.north()).isSolid()) {
		return false;
	}

	if (region.getMaterial(pos.south()).isSolid()) {
		return false;
	}

	BlockID below = region.getBlockID(pos.below());
	return below == Block::mCactus->mID || below == Block::mSand->mID;
}

void CactusBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
// 	entity.mInsideBlockId = region.getBlockID(pos);
}

bool CactusBlock::canBeSilkTouched() const {
	return false;
}
