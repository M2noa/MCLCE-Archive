#include "Dungeons.h"

#include "world/level/levelgen/feature/SandFeature.h"
#include "world/level/material/Material.h"
#include "world/level/block/Block.h"
#include "world/level/BlockSource.h"
#include "world/level/BlockPos.h"
#include "util/Random.h"

SandFeature::SandFeature(BlockID block, int radius)
	: mBlock(block)
	, mRadius(radius) {
}

bool SandFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (region.getMaterial(pos) != Material::getMaterial(MaterialType::Water)) {
		return false;
	}

	int r = random.nextInt(mRadius - 2) + 2;
	int yr = 2;
	for (auto xx : range_incl(pos.x - r, pos.x + r)) {
		for (auto zz : range_incl(pos.z - r, pos.z + r)) {
			int xd = xx - pos.x;
			int zd = zz - pos.z;
			if (xd * xd + zd * zd > r * r) {
				continue;
			}

			for (auto yy : range_incl(pos.y - yr, pos.y + yr)) {
				BlockPos sandPos(xx, yy, zz);
				BlockID t = region.getBlockID(sandPos);
				if (t == Block::mDirt->mID || t == Block::mGrass->mID) {
					_setBlockAndData( region, sandPos, mBlock);
				}
			}
		}
	}

	return true;
}
