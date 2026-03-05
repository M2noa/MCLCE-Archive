/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/BlockPatternBuilder.h"
#include "world/level/block/Block.h"
#include "world/Facing.h"

static const int NO_DROP_BIT = 0x8;

PatternEntry::PatternEntry(BlockID id, std::function<bool(BlockSource&, const BlockPos& pos, BlockID)> blockTester)
	: mID(id)
	, mBlockEntryTester(blockTester) {

}

Unique<BlockPatternBuilder> BlockPatternBuilder::start(BlockSource& region) {
	return make_unique<BlockPatternBuilder>(region);
}

BlockPatternBuilder::BlockPatternBuilder(BlockSource& region)
	: mReadyForMatch(false)
	, mNumPatterns(0)
	, mPatternLength(0)
	, mRegion(region) {
}

BlockPatternBuilder& BlockPatternBuilder::aisle(int n_args, ...) {
	va_list args;
	va_start(args, n_args);
	std::string subPattern = "";
	for (int i = 1; i <= n_args; i++) {
		subPattern = va_arg(args, char*);
		mPattern.push_back(subPattern);
	}
	va_end(args);

	mNumPatterns = n_args;
	mPatternLength = subPattern.size();

	return *this;
}

BlockPatternBuilder& BlockPatternBuilder::define(char pattern, BlockID id, BlockEntryTester tester) {
	mLookup.insert(std::pair<char, PatternEntry>(pattern, PatternEntry(id, tester)));
	return *this;
}

BlockPatternBuilder& BlockPatternBuilder::build() {
	mReadyForMatch = _allCharactersMatched();
	return *this;
}

BuildMatch BlockPatternBuilder::match(const BlockPos& pos) {
	BuildMatch buildMatch;
	buildMatch.mMatched = false;

	if (!mReadyForMatch) {
		return buildMatch;
	}

	int dist = Math::max(mNumPatterns, mPatternLength);

	for (int blockPosX = pos.x - dist; blockPosX <= pos.x; ++blockPosX) {
		for (int blockPosY = pos.y - dist; blockPosY <= pos.y; ++blockPosY) {
			for (int blockPosZ = pos.z - dist; blockPosZ <= pos.z; ++blockPosZ) {
				BlockPos targetPos(blockPosX, blockPosY, blockPosZ);
				BlockID id = mRegion.getBlockID(targetPos);

				for (int x = 0; x < mNumPatterns; ++x) {
					for (int y = 0; y < mPatternLength; ++y) {
						if (_fitsBlockPatternEntry(x, y, targetPos, id)) {
							buildMatch = match(targetPos, x, y);
							if (buildMatch.mMatched) {
								return buildMatch;
							}
						}
					}
				}
			}
		}
	}

	return buildMatch;
}

BuildMatch BlockPatternBuilder::match(const BlockPos& pos, int subPattern, int rowIndex) {
	BuildMatch buildMatch;
	buildMatch.mMatched = false;

	if (!mReadyForMatch) {
		return buildMatch;
	}

	for(FacingID forward = 0; forward < Facing::MAX; ++forward){
		for (FacingID up = 0; up < Facing::MAX; ++up) {
			if (up == forward || up == Facing::OPPOSITE_FACING[forward]) {
				continue;
			}
			buildMatch = match(pos, subPattern, rowIndex, forward, up);
			if (buildMatch.mMatched) {
				return buildMatch;
			}
		}
	}

	return buildMatch;
}

BuildMatch BlockPatternBuilder::match(const BlockPos& pos, int subPattern, int rowIndex, FacingID forward, FacingID up) {
	BuildMatch buildMatch;
	buildMatch.mMatched = false;
	buildMatch.mForward = forward;
	buildMatch.mUp = up;
	buildMatch.mNumPatterns = mNumPatterns;
	buildMatch.mPatternSize = mPatternLength;
	buildMatch.mSubPatternIndex = subPattern;
	buildMatch.mRowIndex = rowIndex;

	int x, y, z;
	buildMatch.mPattern.x = x = pos.x;
	buildMatch.mPattern.y = y = pos.y;
	buildMatch.mPattern.z = z = pos.z;

	if (!mReadyForMatch) {
		return buildMatch;
	}

	for (int i = 0; i < mPatternLength; ++i) {
		for (int j = 0; j < mNumPatterns; ++j) {
			Vec3 forwardVec((float)Facing::getStepX(forward), (float)Facing::getStepY(forward), (float)Facing::getStepZ(forward));
			Vec3 upVec((float)Facing::getStepX(up), (float)Facing::getStepY(up), (float)Facing::getStepZ(up));
			Vec3 rightVec = forwardVec.cross(upVec);
			Vec3 checkVector(0, (float)j, (float)i);

			Vec3 targetPos = pos + Vec3(upVec.x * checkVector.y + rightVec.x * checkVector.z + forwardVec.x * checkVector.x,
				upVec.y * checkVector.y + rightVec.y * checkVector.z + forwardVec.y * checkVector.x, 
				upVec.z * checkVector.y + rightVec.z * checkVector.z + forwardVec.z * checkVector.x);
			if (!_fitsBlockPatternEntry(j, i, targetPos)) {
				return buildMatch;
			}
		}
	}

	buildMatch.mMatched = true;
	buildMatch.mObjectPos = Vec3(
		(float)buildMatch.mPattern.x + (mPatternLength - 1) / 2.0f,
		(float)buildMatch.mPattern.y - (mNumPatterns - 1),
		(float)buildMatch.mPattern.z + (mPatternLength - 1) / 2.0f
		);
	return buildMatch;
}

void BlockPatternBuilder::replaceBlocks(char subPattern, BuildMatch buildMatch, BlockID id, bool dontDrop) {
	const BlockPos& pos = buildMatch.mPattern;

	for (int i = 0; i < buildMatch.mPatternSize; ++i) {
		for (int j = 0; j < buildMatch.mNumPatterns; ++j) {

			Vec3 forwardVec((float)Facing::getStepX(buildMatch.mForward), (float)Facing::getStepY(buildMatch.mForward), (float)Facing::getStepZ(buildMatch.mForward));
			Vec3 upVec((float)Facing::getStepX(buildMatch.mUp), (float)Facing::getStepY(buildMatch.mUp), (float)Facing::getStepZ(buildMatch.mUp));
			Vec3 rightVec = forwardVec.cross(upVec);
			Vec3 checkVector(0, (float)j, (float)i);

			if (_isAnyPattern(subPattern) || subPattern == _getPattern(j, i)) {
				Vec3 targetPos = pos + Vec3(upVec.x * checkVector.y + rightVec.x * checkVector.z + forwardVec.x * checkVector.x,
					upVec.y * checkVector.y + rightVec.y * checkVector.z + forwardVec.y * checkVector.x,
					upVec.z * checkVector.y + rightVec.z * checkVector.z + forwardVec.z * checkVector.x);

				if (dontDrop) {
					mRegion.setBlockAndData(targetPos, {mRegion.getBlock(targetPos).mID, DataID(NO_DROP_BIT)}, Block::UPDATE_ALL);
				}
				mRegion.setBlockAndData(targetPos, id, Block::UPDATE_ALL);

				
			}
		}
	}
}

bool BlockPatternBuilder::isReadyForMatch() {
	return mReadyForMatch;
}

bool BlockPatternBuilder::_allCharactersMatched() {
	for (auto& subPattern : mPattern) {
		for (auto& c : subPattern) {
			if (mLookup.find(c) == mLookup.end() && !_isAnyPattern(c)) {
				return false;
			}
		}
	}

	return true;
}

PatternEntry BlockPatternBuilder::_getBlockPatternEntry(int subPattern, int rowIndex) {
	return mLookup.find(_getPattern(subPattern, rowIndex))->second;
}

char BlockPatternBuilder::_getPattern(int subPattern, int rowIndex) {
	DEBUG_ASSERT(subPattern < mNumPatterns && rowIndex < mPatternLength, "Index doesn't match!");
	return mPattern[subPattern][rowIndex];
}

bool BlockPatternBuilder::_isAnyPattern(char pattern) {
	return pattern == ANY_PATTERN;
}

bool BlockPatternBuilder::_fitsAnyPattern(int subPattern, int rowIndex) {
	return _isAnyPattern(_getPattern(subPattern, rowIndex));
}

bool BlockPatternBuilder::_fitsBlockPatternEntry(int subPattern, int rowIndex, const BlockPos& pos) {
	BlockID id = mRegion.getBlockID(pos);
	return _fitsBlockPatternEntry(subPattern, rowIndex, pos, id);
}

bool BlockPatternBuilder::_fitsBlockPatternEntry(int subPattern, int rowIndex, const BlockPos& pos, BlockID id) {
	if (_fitsAnyPattern(subPattern, rowIndex)) {
		return true;
	}

	PatternEntry entry = _getBlockPatternEntry(subPattern, rowIndex);
	if (entry.mBlockEntryTester != nullptr) {
		return entry.mBlockEntryTester(mRegion, pos, id);
	}

	return entry.mID == id;
}
