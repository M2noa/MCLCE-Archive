#include "Dungeons.h"

#include "world/level/levelgen/feature/ReedsFeature.h"
#include "world/level/block/Block.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "util/Random.h"

bool ReedsFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	for (int i = 0; i < 20; i++) {
		const int randX = random.nextGaussianInt(8);
		const int randZ = random.nextGaussianInt(8);
		BlockPos blockPos = pos.offset(randX, 0, randZ);

		if (region.isEmptyBlock(blockPos)) {
			BlockPos belowBlock = blockPos.below();
			if (region.getMaterial(belowBlock.west()).isType(MaterialType::Water) ||
				region.getMaterial(belowBlock.east()).isType(MaterialType::Water) ||
				region.getMaterial(belowBlock.north()).isType(MaterialType::Water) ||
				region.getMaterial(belowBlock.south()).isType(MaterialType::Water)) {

				int randh = random.nextInt(3) + 1;
				int h = 2 + random.nextInt(randh);
				for (int yy = 0; yy < h; yy++) {
					if (Block::mReeds->canSurvive(region, blockPos.above(yy))) {
						_setBlockAndData( region, {blockPos.x, blockPos.y + yy, blockPos.z}, Block::mReeds->mID);
					}
				}
			}
		}
	}

	return true;
}
