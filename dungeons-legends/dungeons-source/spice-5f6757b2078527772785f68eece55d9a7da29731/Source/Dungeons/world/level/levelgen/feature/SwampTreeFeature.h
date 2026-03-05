/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/TreeFeature.h"

class SwampTreeFeature : public TreeFeature {
public:

	SwampTreeFeature();
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

private:

	void _addVine(BlockSource& region, BlockPos pos, int dir) const;
};
