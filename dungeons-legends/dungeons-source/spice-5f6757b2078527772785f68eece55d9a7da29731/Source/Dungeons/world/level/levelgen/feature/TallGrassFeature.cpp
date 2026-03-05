#include "Dungeons.h"

#include "world/level/levelgen/feature/TallGrassFeature.h"
#include "world/level/block/TallGrass.h"
#include "world/level/block/LeafBlock.h"
#include "world/level/BlockSource.h"

TallGrassFeature::TallGrassFeature(BlockID block, DataID type)
	: mGrass(block, type) {
}

bool TallGrassFeature::place(BlockSource& region, const BlockPos& pos, Random& random, int tries, int r) const {
	BlockPos blockPos(pos);
	BlockID t = BlockID::AIR;

	while (((t = region.getBlockID(blockPos)) == 0 || t.hasProperty(BlockProperty::Leaf)) && blockPos.y > 0) {
		blockPos.y--;
	}

	for (int i = 0; i < tries; i++) {
		const int randZ = random.nextGaussianInt(r);
		const int randY = random.nextGaussianInt(4);
		const int randX = random.nextGaussianInt(r);
		BlockPos grassPos = pos.offset(randX, randY, randZ);

		if (region.getBlockID(grassPos) == BlockID::AIR && Block::mBlocks[mGrass.id]->canSurvive(region, grassPos)) {
			_setBlockAndData(region, grassPos, mGrass);
		}
	}

	return true;
}

bool TallGrassFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	return place(region, pos, random, 90, 8);
}
