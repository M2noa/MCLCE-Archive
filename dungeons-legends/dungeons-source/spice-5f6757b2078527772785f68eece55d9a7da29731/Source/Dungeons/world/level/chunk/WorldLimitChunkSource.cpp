/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/WorldLimitChunkSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/block/Block.h"
#include "world/level/dimension/Dimension.h"
#include "nbt/CompoundTag.h"

WorldLimitChunkSource::WorldLimitChunkSource(Unique<ChunkSource> storage, const BlockPos& center) 
	: ChunkSource(std::move(storage))
	, mLimitArea(ChunkPos(center) - 8, ChunkPos(center) + 7){

	//create the invisible bedrock wall
	mInvisibleWallChunk = make_unique<LevelChunk>(*mLevel, *mDimension, ChunkPos::INVALID, true);

	std::vector<BlockID> blocks(getDimension().getHeight() * CHUNK_COLUMNS, Block::mInvisibleBedrock->mID);

	mInvisibleWallChunk->setAllBlockIDs(blocks, getDimension().getHeight());
	mInvisibleWallChunk->changeState(ChunkState::Unloaded, ChunkState::Loaded);

	//acquire all the chunks in the bounds upfront
	for (auto& pos : mLimitArea) {
		mAcquiredChunks.push_back(mParent->requestChunk(ChunkPos(pos.x, pos.z), LoadMode::Deferred));
	}
}

WorldLimitChunkSource::~WorldLimitChunkSource() {

	for (auto& c : mAcquiredChunks) {
		mParent->releaseChunk(*c);
	}
}

LevelChunk* WorldLimitChunkSource::requestChunk(const ChunkPos& cp, LoadMode lm ){
	if (mLimitArea.contains(Pos(cp.x, 0, cp.z))) {
		return mParent->requestChunk(cp, lm);
	}
	else {
		return mInvisibleWallChunk.get();
	}
}

LevelChunk* WorldLimitChunkSource::getExistingChunk(const ChunkPos& cp){
	if (mLimitArea.contains(Pos(cp.x, 0, cp.z))) {
		return mParent->getExistingChunk(cp);
	}
	else {
		return mInvisibleWallChunk.get();
	}
}

bool WorldLimitChunkSource::releaseChunk(LevelChunk& lc){
	if (&lc != mInvisibleWallChunk.get()) {
		return mParent->releaseChunk(lc);
	}
	else {
		return false;
	}
}

Unique<CompoundTag> WorldLimitChunkSource::getStructureTag() const {
	return make_unique<CompoundTag>();
}

void WorldLimitChunkSource::readStructureTag(const CompoundTag& tag) {
	UNUSED_PARAMETER(tag)
}
