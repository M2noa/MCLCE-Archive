/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/levelgen/feature/Feature.h"
#include "world/phys/AABB.h"

class BlockPos;
class BlockSource;
class Random;

class SpikeFeature : public Feature {
public:

	class EndSpike {
	public:
		EndSpike(int centerX, int centerZ, int radius, int height, bool guarded);

		bool startsInChunk(const BlockPos& chunkOrigin) const;

		int getCenterX() const;
		int getCenterZ() const;
		int getRadius() const;
		int getHeight() const;
		bool isGuarded() const;
		AABB getTopBoundingBox() const;

	private:
		const int mCenterX;
		const int mCenterZ;
		const int mRadius;
		const int mHeight;
		const bool mGuarded;
		const AABB mTopBoundingBox;
	};

	SpikeFeature(const EndSpike& spike);

	void setCrystalInvulnerable(bool invulnerable);
	void setCrystalBeamTarget(const BlockPos& target);

	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;
	bool placeManually(BlockSource& region, const BlockPos& pos, Random& random, Entity* placer);
	void postProcessMobsAt(BlockSource& region, const BlockPos& pos, Random& random) const;

private:
	const EndSpike& mSpike;
	BlockPos mCrystalBeamTarget;
	bool mCrystalInvulnerable;
};
