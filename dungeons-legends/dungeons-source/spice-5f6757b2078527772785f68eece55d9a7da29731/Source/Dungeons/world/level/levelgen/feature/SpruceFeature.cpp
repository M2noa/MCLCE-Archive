#include "Dungeons.h"

#include "world/level/levelgen/feature/SpruceFeature.h"
#include "world/level/block/OldLeafBlock.h"
#include "world/level/block/OldLogBlock.h"

SpruceFeature::SpruceFeature(Entity* placer)
	: TreeFeature(placer, enum_cast(OldLogBlock::LogType::Spruce), enum_cast(OldLeafBlock::LeafType::Spruce)) {
}

bool SpruceFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	// pines can be quite tall
	int treeHeight = random.nextInt(4) + 6;
	if (!_prepareSpawn(region, pos, treeHeight)) {
		return false;
	}

	int trunkHeight = 1 + random.nextInt(2);
	int topHeight = treeHeight - trunkHeight;
	int leafRadius = 2 + random.nextInt(2);

	if (!_placeTrunk(region, pos, random, treeHeight - random.nextInt(3))) {
		return true;
	}

	// place leaf top
	int currentRadius = random.nextInt(2);
	int maxRadius = 1;
	int minRadius = 0;

	for (int heightPos = 0; heightPos <= topHeight; heightPos++) {
		const int yy = pos.y + treeHeight - heightPos;

		for (int xx = pos.x - currentRadius; xx <= pos.x + currentRadius; xx++) {
			int xo = xx - pos.x;

			for (int zz = pos.z - currentRadius; zz <= pos.z + currentRadius; zz++) {
				int zo = zz - pos.z;
				if (std::abs(xo) == currentRadius && std::abs(zo) == currentRadius && currentRadius > 0) {
					continue;
				}
				_placeLeaf(region, BlockPos(xx, yy, zz));
			}
		}

		if (currentRadius >= maxRadius) {
			currentRadius = minRadius;
			minRadius = 1;
			maxRadius += 1;
			if (maxRadius > leafRadius) {
				maxRadius = leafRadius;
			}
		}
		else {
			currentRadius = currentRadius + 1;
		}
	}

	return true;
}
