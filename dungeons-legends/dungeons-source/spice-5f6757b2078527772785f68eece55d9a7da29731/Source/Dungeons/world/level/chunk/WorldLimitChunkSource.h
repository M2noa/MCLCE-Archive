/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/chunk/ChunkSource.h"

class WorldLimitChunkSource :
	public ChunkSource {
public:
	WorldLimitChunkSource(Unique<ChunkSource> storage, const BlockPos& center);
	virtual ~WorldLimitChunkSource();

	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm ) override;

	virtual bool releaseChunk(LevelChunk& lc) override;

	virtual LevelChunk* getExistingChunk(const ChunkPos& cp) override;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

private:

	std::vector<LevelChunk*> mAcquiredChunks;
	Unique<LevelChunk> mInvisibleWallChunk;
	const Bounds mLimitArea;
};
