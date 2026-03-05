/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/levelgen/feature/TreeFeature.h"
#include "world/level/BlockPos.h"

class FancyTreeFeature : public TreeFeature {
protected:

	class FoliageCoords : public BlockPos {
	public:

		FoliageCoords(BlockPos pos, int branchBase);
		int getBranchBase() const;

	private:
		int mBranchBase;
	};

public:

	FancyTreeFeature(Entity* placer = nullptr);
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;

protected:

	float _getTreeShape(int height, int y) const;
	bool _checkLocation(BlockSource& region, const BlockPos& origin, int& height) const;
	int _checkLine(BlockSource& region, const BlockPos& startPos, const BlockPos& endPos) const;
	int _getSteps(const BlockPos& pos) const;
	void _makeFoliage(BlockSource& region, const std::vector<FoliageCoords>& foliageCoords) const;
	void _foliageCluster(BlockSource& region, const FoliageCoords& foliageCoord) const;
	float _getFoliageShape(int y) const;
	void _crossection(BlockSource& region, const BlockPos& pos, float radius, const BlockID block) const;
	void _makeTrunk(BlockSource& region, const BlockPos& origin, int trunkHeight) const;
	void _limb(BlockSource& region, const BlockPos& startPos, const BlockPos& endPos, const BlockID material) const;
	int _getLogFacingData(const BlockPos& startPos, const BlockPos& blockPos) const;
	void _makeBranches(BlockSource& region, const std::vector<FoliageCoords>& foliageCoords, const BlockPos& origin, int height) const;
	bool _trimBranches(int height, int localY) const;

private:

	const int mHeightVariance = 12;
	const float mTrunkHeightScale = 0.618f;
	const float mBranchSlope = 0.381f;
	const float mWidthScale = 1;
	const float mFoliageDensity = 1;
	const int mTrunkWidth = 1;
	const int mFoliageHeight = 4;
	const int mMinAllowedHeight = 6;
};
