/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "CommonTypes.h"
#include "world/level/ChunkPos.h"

class LevelChunk;

class ChunkRefCount {
public:

	typedef Unique<LevelChunk> ChunkPtr;
	typedef std::pair<ChunkPos, ChunkRefCount> MapPair;
	typedef std::unordered_map<ChunkPos, ChunkRefCount> Map;

	ChunkRefCount();
	ChunkRefCount(LevelChunk& lc, int initial = 1 );
	ChunkRefCount(ChunkPtr&& rhs, int initial = 1);
	ChunkRefCount(ChunkRefCount&& rhs);

	~ChunkRefCount();

	ChunkRefCount& operator=(ChunkRefCount&& rhs);

	LevelChunk* get() const;

	LevelChunk* operator->() const;

	LevelChunk* grab();

	int release();

	int getRefCount() const;
	LevelChunk& getChunk();
	const LevelChunk& getChunk() const;

	ChunkPtr& getChunkPtr();
protected:
	int mRefCount;
	ChunkPtr mChunk;
};
