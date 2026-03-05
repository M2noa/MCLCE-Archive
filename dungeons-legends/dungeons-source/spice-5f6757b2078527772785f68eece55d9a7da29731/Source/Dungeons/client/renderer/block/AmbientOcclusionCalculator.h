/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "Core/Math/Color.h"
#include "world/Facing.h"

class Block;
class BlockTessellatorCache;

class AmbientOcclusionCalculator {
public:
	AmbientOcclusionCalculator(const BlockPos& centerPos, const Block& centerBlock, BlockTessellatorCache& cache);
	void setBaseColor(const Color& color);
	void setOutputColors(Color* colors);
	void setOutputLightTexturePositions(BrightnessPair* lightTexturePositions);
	void setTintSides(bool tint);
	void setSeasons(bool seasons);
	void setTouchEdge(bool touch);
	void calculate(FacingID face, bool translucent);

	float getShadingForFace(FacingID face);
	bool isTranslucent(const BlockPos& pos);

private:
	float _getShadeBrightness(const BlockPos& pos);
	BrightnessPair _blend(BrightnessPair a, BrightnessPair b, BrightnessPair c, BrightnessPair def);
	bool _isSolidRender(const BlockPos& pos);
	bool _notOccludedBy(const BlockPos& neighbour1, const BlockPos& neighbour2);
	Color _computeAOColor(float baseAO, bool translucent, float gamma) const;

	bool mTintSides = false;
	bool mDoSeasons = false;
	bool mTouchEdge = true;

	float mFaceShading[6];

	BlockTessellatorCache& mCache;
	const BlockPos& mCenterPos;
	const Block& mCenterBlock;	//	@note: For some reason this isn't being used, is that intentional?
	Color mBaseColor = Color::WHITE;
	BrightnessPair* mOutputLightTexturePositons = nullptr;
	Color* mOutputColors = nullptr;
};
