#include "Dungeons.h"

#include "world/level/levelgen/feature/VinesFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/LevelConstants.h"
#include "world/Facing.h"
#include "world/Direction.h"
#include "util/Random.h"

bool VinesFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	BlockPos blockPos(pos);
	while (blockPos.y < region.getMaxHeight()) {
		if (region.isEmptyBlock(blockPos)) {
			for (FacingID face = Facing::NORTH; face <= Facing::EAST; ++face) {
				if (Block::mVine->mayPlace(region, blockPos, face)) {
					int faceOpposite = 1 << Direction::FACING_DIRECTION[Facing::OPPOSITE_FACING[face]];

					DataID data = 0;
					Block::mVine->getBlockState(BlockState::VineGrowth).set(data, faceOpposite);

					_setBlockAndData(region, blockPos, {Block::mVine->mID, data});
					break;
				}
			}
		}
		else {
			blockPos.x = pos.x + random.nextGaussianInt(4);
			blockPos.z = pos.z + random.nextGaussianInt(4);
		}
		blockPos.y++;
	}

	return true;
}
