/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/chunk/ChunkSource.h"

#include "world/level/chunk/ChunkRefCount.h"
// #include "AppPlatformListener.h"

class NetworkChunkSource :
	public ChunkSource
// 	, public AppPlatformListener 
{
public:

	NetworkChunkSource(Level* level, Dimension* dimension);

	virtual LevelChunk* getExistingChunk(const ChunkPos& cp) override;

	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm) override;
	virtual bool releaseChunk(LevelChunk& lc) override;

	virtual const ChunkRefCount::Map& getStoredChunks() const override;
	virtual ChunkRefCount::Map& getStoredChunks() override;

	virtual void acquireDiscarded(ChunkPtr&& ptr) override;

	LevelChunk& getIncomingChunk(const ChunkPos& cp);

	virtual void onLowMemory() /*override*/;

	void updateCachedData(LevelChunk* lc);

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

protected:

	std::unordered_map<ChunkPos, ChunkPtr> mIncomingChunks;
	ChunkRefCount::Map mChunkMap;
};
