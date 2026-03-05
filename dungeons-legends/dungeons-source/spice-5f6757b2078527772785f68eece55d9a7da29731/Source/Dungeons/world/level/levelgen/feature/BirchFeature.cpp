#include "Dungeons.h"

#include "world/level/levelgen/feature/BirchFeature.h"
#include "world/level/block/OldLeafBlock.h"
#include "world/level/block/OldLogBlock.h"
#include "util/Random.h"

BirchFeature::BirchFeature(Entity* placer, bool superBirch) :
	TreeFeature(placer, enum_cast(OldLogBlock::LogType::Birch), enum_cast(OldLeafBlock::LeafType::Birch))
	, mSuperBirch(superBirch) {

}

bool BirchFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	int treeHeight = random.nextInt(3) + 5;
	if (mSuperBirch) {
		treeHeight += random.nextInt(7);
	}

	return TreeFeature::place(region, pos, random, treeHeight);
}
