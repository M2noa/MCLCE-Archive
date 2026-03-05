#pragma once

#include "common_header.h"

#include "ChunkSource.h"

class EmptyChunkSource : public ChunkSource {
public:

	EmptyChunkSource(Level* level, Dimension* dimension);

	LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm) override;

	void loadChunk(LevelChunk& lc, bool forceImmediateReplacementDataLoad) override;

	bool postProcess(ChunkViewSource& neighborhood) override;

	virtual Unique<CompoundTag> getStructureTag() const override;
	virtual void readStructureTag(const CompoundTag& tag) override;

};
