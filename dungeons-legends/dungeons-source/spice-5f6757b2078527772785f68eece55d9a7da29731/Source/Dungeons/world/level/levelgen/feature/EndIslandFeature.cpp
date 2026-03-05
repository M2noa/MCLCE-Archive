#include "Dungeons.h"

#include "world/level/levelgen/feature/EndIslandFeature.h"

#include "util/Random.h"
#include "world/level/block/Block.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"

bool EndIslandFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	float size = random.nextInt(3) + 4.0f;
	int y = 0;

	while (size > 0.5f) {
		for (int x = Math::floor(-size); x <= Math::ceil(size); ++x) {
			for (int z = Math::floor(-size); z <= Math::ceil(size); ++z) {
				if (x * x + z * z <= (size + 1) * (size + 1)) {
					_setBlockAndData( region, pos.offset(x, y, z), Block::mEndStone->mID);
				}
			}
		}
		size -= random.nextInt(2) + 0.5f;
		--y;
	}

	return true;
}
