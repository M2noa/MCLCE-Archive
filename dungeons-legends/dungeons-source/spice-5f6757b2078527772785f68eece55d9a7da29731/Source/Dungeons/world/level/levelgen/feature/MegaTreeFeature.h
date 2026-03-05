/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/TreeFeature.h"
#include "world/level/LevelConstants.h"
/**
 * Same as tree feature, but slightly taller and white in color.
 */
class MegaTreeFeature : public TreeFeature {
public:

	MegaTreeFeature(Entity* placer, int baseHeight, int heightInterval, int trunkType, int leafType);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

protected:

	int _calcTreeHeigth(Random& random) const;
	bool _checkIsFree(BlockSource& region, const BlockPos& pos, int treeHeight) const;
	bool _makeDirtFloor(BlockSource& region, const BlockPos& pos) const;
	bool _prepareTree(BlockSource& region, const BlockPos& pos, int treeHeight) const;
	void _placeDoubleTrunkLeaves(BlockSource& region, const BlockPos& origin, int radius) const;
	void _placeSingleTrunkLeaves(BlockSource& region, const BlockPos& origin, int radius) const;

	int _getBaseHeight() const;
	int _getHeightInterval() const;
	int _getTrunkType() const;
	int _getLeafType() const;

private:

	const int mBaseHeight;
	const int mHeightInterval;
	const int mTrunkType;
	const int mLeafType;
};
