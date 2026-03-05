#include "Dungeons.h"

#include "world/level/levelgen/feature/JungleTreeFeature.h"
#include "world/level/block/OldLeafBlock.h"
#include "world/level/block/OldLogBlock.h"

JungleTreeFeature::JungleTreeFeature(Entity* placer) 
	: TreeFeature(placer, enum_cast(OldLogBlock::LogType::Jungle), enum_cast(OldLeafBlock::LeafType::Jungle), true) {

}

bool JungleTreeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	int treeHeight = random.nextInt(7) + 4;
	return TreeFeature::place(region, pos, random, treeHeight);
}
