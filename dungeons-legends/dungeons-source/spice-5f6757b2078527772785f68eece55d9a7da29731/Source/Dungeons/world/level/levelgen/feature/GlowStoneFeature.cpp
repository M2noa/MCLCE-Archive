#include "Dungeons.h"

#include "world/level/levelgen/feature/GlowStoneFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/material/Material.h"
#include "util/Random.h"

bool GlowStoneFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (!region.isEmptyBlock(pos)) {
		return false;
	}
	if (region.getBlockID(pos.above()) != Block::mNetherrack->mID) {
		return false;
	}

	_setBlockAndData( region, pos, Block::mGlowStone->mID);

	for (int i = 0; i < 1500; i++) {
		const int randZ = random.nextGaussianInt(8);
		const int randY = -random.nextInt(12);
		const int randX = random.nextGaussianInt(8);
		BlockPos gemPos = pos.offset(randX, randY, randZ);

		if (!region.getMaterial(gemPos).isType(MaterialType::Air)) {
			continue;
		}

		int count = 0;

		for (int t = 0; t < 6; t++) {
			BlockID block(BlockID::AIR);
			switch (t) {
			case 0:
				block = region.getBlockID(gemPos.west());
				break;
			case 1:
				block = region.getBlockID(gemPos.east());
				break;
			case 2:
				block = region.getBlockID(gemPos.below());
				break;
			case 3:
				block = region.getBlockID(gemPos.above());
				break;
			case 4:
				block = region.getBlockID(gemPos.north());
				break;
			case 5:
				block = region.getBlockID(gemPos.south());
				break;
			}

			if (block == Block::mGlowStone->mID) {
				count++;
			}
		}

		if (count == 1) {
			_setBlockAndData( region, gemPos, Block::mGlowStone->mID);
		}
	}

	return true;
}
