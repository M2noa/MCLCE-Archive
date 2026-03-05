#pragma once

#include "common_header.h"

#include "world/level/chunk/ChunkSource.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "io/ObjectGroupFile.h"
#include "util/NibblePair.h"

class Block;
class Dimension;

class LovikaLevelSource : public ChunkSource {
public:

	LovikaLevelSource(Level* level, Dimension* dimension, io::ObjectGroup* objGroup);

	virtual LevelChunk* getExistingChunk(const ChunkPos& cp) override;

	virtual void loadChunk(LevelChunk& c, bool forceImmediateReplacementDataLoad) override;

	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm) override;

	virtual bool postProcess(ChunkViewSource& chunks) override;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

private:

	io::ObjectGroup* objGroup;

	std::vector<BlockID> mPrototype;
	std::vector<NibblePair> mPrototypeData;

	ChunkRefCount::Map mChunkMap;
};
