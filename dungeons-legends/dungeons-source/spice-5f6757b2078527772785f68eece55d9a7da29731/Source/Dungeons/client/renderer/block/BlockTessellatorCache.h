/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/BlockPos.h"
#include "world/level/LevelConstants.h"
class Block;
class BlockSource;

class BlockTessellatorCache {
public:
	static const int CACHE_SIDE_X = RENDERCHUNK_SIDE + 4;
	static const int CACHE_SIDE_Y = RENDERCHUNK_SIDE + 4;
	static const int CACHE_SIDE_Z = RENDERCHUNK_SIDE + 4;

	void reset(BlockSource& region, const BlockPos& startPostion);
	BrightnessPair getLightColor(const BlockPos& p);
	const Block& getBlock(const BlockPos& p);
	DataID getData(const BlockPos& pos);
	BlockSource* getRegion() {
		return mRegion;
	}

	void setBlockAtPosition(const Block& block, BlockPos& p);

private:
	Brightness _getLightEmission(const Block& block);
	BlockPos _getPosInArray(const BlockPos& p);
	int _getIndexFromPositionInArray(const BlockPos& p);

	static const Block& mUnsetBlockValue;
	BlockPos pos = BlockPos::ZERO;
	BlockSource* mRegion = nullptr;
	std::array<BrightnessPair, CACHE_SIDE_X * CACHE_SIDE_Y * CACHE_SIDE_Z> mLightColors;
	std::array<const Block*, CACHE_SIDE_X * CACHE_SIDE_Y * CACHE_SIDE_Z> mBlocks;
	std::array<DataID, CACHE_SIDE_X * CACHE_SIDE_Y * CACHE_SIDE_Z> mData;
};
