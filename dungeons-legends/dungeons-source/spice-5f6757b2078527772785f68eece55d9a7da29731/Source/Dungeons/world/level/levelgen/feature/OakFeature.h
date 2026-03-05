/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/TreeFeature.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/block/LeafBlock.h"
#include "util/Random.h"

class OakFeature : public TreeFeature {
public:

	OakFeature(Entity* placer = nullptr)
		: TreeFeature(placer, enum_cast(OldLogBlock::LogType::Oak), LeafBlock::NORMAL_LEAF) {

	}

	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override {
		int treeHeight = random.nextInt(3) + 4;
		return TreeFeature::place(region, pos, random, treeHeight);
	}
};
