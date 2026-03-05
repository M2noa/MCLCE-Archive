#include "Dungeons.h"

#include "world/level/levelgen/feature/DeadBushFeature.h"
#include "world/level/material/Material.h"
#include "world/level/block/Block.h"
#include "world/level/BlockSource.h"
#include "util/Random.h"

bool DeadBushFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	BlockID t(0);
	BlockPos blockPos(pos);
	const Material* currentMaterial = nullptr;

	while (t != Block::mDeadBush->mID && (currentMaterial == nullptr || _isFree(*currentMaterial)) && blockPos.y > 0) {
		currentMaterial = &region.getMaterial(blockPos);
		blockPos = blockPos.below();
	}

	for (auto i : range(4)) {
		UNUSED_VARIABLE(i);
		const int randZ = random.nextGaussianInt(8);
		const int randY = random.nextGaussianInt(4);
		const int randX = random.nextGaussianInt(8);
		BlockPos bushPos = blockPos.offset(randX, randY, randZ);

		if (region.isEmptyBlock(bushPos)) {
			if (Block::mDeadBush->canSurvive(region, bushPos)) {
				_setBlockAndData( region, bushPos, Block::mDeadBush->mID);
			}
		}
	}

	return true;
}

bool DeadBushFeature::_isFree(const Material& material) const {
	return material.isType(MaterialType::Air)
		|| material.isType(MaterialType::Leaves);
}
