/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/MegaTreeFeature.h"

/**
 * Same as tree feature, but slightly taller and white in color.
 */
class MegaPineTreeFeature : public MegaTreeFeature {
public:

	MegaPineTreeFeature(Entity* placer, bool isSpruce);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

	void createCrown(BlockSource& region, const BlockPos& origin, int baseRadius, Random& random) const;
	void postPlaceTree(BlockSource& region, Random& random, const BlockPos& pos) const;

private:

	void _placePodzolCircle(BlockSource& region, const BlockPos& pos) const;
	void _placePodzolAt(BlockSource& region, const BlockPos& pos) const;

	bool mIsSpruce;
};
