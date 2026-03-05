/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/MushroomBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/levelgen/feature/HugeMushroomFeature.h"
#include "world/level/LevelConstants.h"

MushroomBlock::MushroomBlock(const std::string& nameId, int id)
	: BushBlock(nameId, id, Material::getMaterial(MaterialType::Plant)) {
	float ss = 0.2f;
	setVisualShape( Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, ss * 2, 0.5f + ss));
	setTicking(true);
	mRenderLayer = RENDERLAYER_ALPHATEST;
}

void MushroomBlock::tick(BlockSource& region, const BlockPos& pos, Random& random ) const{
	if(random.nextInt(25) == 0) {
		int r = 4;
		int max = 5;
		BlockPos newPos(pos);

		for (int xx = pos.x - r; xx <= pos.x + r; xx++) {
			for (int zz = pos.z - r; zz <= pos.z + r; zz++) {
				for (int yy = pos.y - 1; yy <= pos.y + 1; yy++) {
					if (region.getBlockID(xx, yy, zz) == mID && --max <= 0) {
						return;
					}
				}
			}
		}

		int x2 = pos.x + random.nextInt(3) - 1;
		const int randY0 = pos.y + random.nextInt(2);
		int y2 = randY0 - random.nextInt(2);
		int z2 = pos.z + random.nextInt(3) - 1;

		for (int i = 0; i < 4; i++) {
			if (region.isEmptyBlock(x2, y2, z2) && canSurvive(region, BlockPos(x2, y2, z2))) {
				newPos.x = x2;
				newPos.y = y2;
				newPos.z = z2;
			}

			x2 = newPos.x + random.nextInt(3) - 1;
			const int randY1 = newPos.y + random.nextInt(2);
			y2 = randY1 - random.nextInt(2);
			z2 = newPos.z + random.nextInt(3) - 1;
		}

		if (region.isEmptyBlock(x2, y2, z2) && canSurvive(region, BlockPos(x2, y2, z2))) {
			region.setBlock(x2, y2, z2, mID, Block::UPDATE_ALL);
		}
	}
}

bool MushroomBlock::mayPlace( BlockSource& region, const BlockPos& pos, FacingID face ) const {
	return BushBlock::mayPlace(region, pos, face) && canSurvive(region, pos);
}

bool MushroomBlock::mayPlaceOn(const Block& block) const {
	return Block::mSolid[block.mID];
}

bool MushroomBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	if (pos.y < 0 || pos.y >= region.getMaxHeight()) {
		return false;
	}

	const Block& belowBlock = region.getBlock(pos.below());
	return belowBlock.isType(Block::mPodzol) || belowBlock.isType(Block::mMycelium) || (region.getRawBrightness(pos) < 13 && mayPlaceOn(belowBlock));
}

bool MushroomBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	if(!region.getLevel().isClientSide() && region.getLevel().getRandom().nextFloat() < 0.4f) {
		_growTree(region, pos, region.getLevel().getRandom());
	}

	return true;
}

void MushroomBlock::_growTree(BlockSource& region, const BlockPos& pos, Random& random) const {
	region.removeBlock(pos.x, pos.y, pos.z);

	if(isType(Block::mBrownMushroom) || isType(Block::mRedMushroom)) {
		HugeMushroomFeature f(this == Block::mBrownMushroom ? 0 : 1);
		if(!f.place(region, pos, random)) {
			region.setBlockAndData(pos, mID, region.getData(pos), Block::UPDATE_ALL);
		}
	}
}
