/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/chunk/ChunkSource.h"
#include "world/level/LevelConstants.h"
// #include "AppPlatformListener.h"
#include "util/GridArea.h"
#include "world/level/chunk/ChunkRefCount.h"

class LevelChunk;
enum class StructureFeatureType : int8_t;

class MainChunkSource :
	public ChunkSource {
public:

	MainChunkSource(Unique<ChunkSource> storage);

	virtual ~MainChunkSource();

	///returns a chunk in the active pool - doesn't use cache (TODO) or disk
	LevelChunk* getExistingChunk(const ChunkPos& cp) override;

	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm) override;

	virtual bool releaseChunk(LevelChunk& lc) override;

	virtual const ChunkRefCount::Map& getStoredChunks() const override;
	virtual ChunkRefCount::Map& getStoredChunks() override;

	virtual void acquireDiscarded(ChunkPtr&& ptr) override;
	virtual bool findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) override;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

private:

	ChunkRefCount::Map mChunkMap;
};
