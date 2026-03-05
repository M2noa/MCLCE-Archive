/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include"world/level/BlockSource.h"
#include "world/level/BlockPos.h"
#include "world/phys/Vec3.h"

struct BuildMatch {
	bool mMatched;
	FacingID mForward;
	FacingID mUp;

	int mNumPatterns;
	int mPatternSize;

	int mSubPatternIndex;
	int mRowIndex;

	BlockPos mPattern;
	Vec3 mObjectPos;
};

class PatternEntry {
public:
	typedef std::function<bool(BlockSource&, const BlockPos&, BlockID)> BlockEntryTester;
	PatternEntry(BlockID id, BlockEntryTester blockTester);

	BlockID mID;
	BlockEntryTester mBlockEntryTester;
};

class BlockPatternBuilder {
public:
	typedef std::function<bool(BlockSource&, const BlockPos&, BlockID)> BlockEntryTester;

	static Unique<BlockPatternBuilder> start(BlockSource& region);

	BlockPatternBuilder(BlockSource& region);

	BlockPatternBuilder& aisle(int n_args, ...);
	BlockPatternBuilder& define(char pattern, BlockID id, BlockEntryTester tester = nullptr);
	BlockPatternBuilder& build();

	BuildMatch match(const BlockPos& pos);
	BuildMatch match(const BlockPos& pos, int subPattern, int rowIndex);
	BuildMatch match(const BlockPos& pos, int subPattern, int rowIndex, FacingID forward, FacingID up);

	void replaceBlocks(char subPattern, BuildMatch buildMatch, BlockID id = BlockID::AIR, bool dontDrop = false);

	bool isReadyForMatch();

private:
	bool _allCharactersMatched();
	PatternEntry _getBlockPatternEntry(int subPattern, int rowIndex);
	char _getPattern(int subPattern, int rowIndex);
	bool _isAnyPattern(char pattern);
	bool _fitsAnyPattern(int subPattern, int rowIndex);
	bool _fitsBlockPatternEntry(int subPattern, int rowIndex, const BlockPos& pos);
	bool _fitsBlockPatternEntry(int subPattern, int rowIndex, const BlockPos& pos, BlockID id);

	static const char ANY_PATTERN = ' ';

	BlockSource& mRegion;
	std::vector<std::string> mPattern;
	std::map<char, PatternEntry> mLookup;

	bool mReadyForMatch;
	int mNumPatterns;
	int mPatternLength;
};
