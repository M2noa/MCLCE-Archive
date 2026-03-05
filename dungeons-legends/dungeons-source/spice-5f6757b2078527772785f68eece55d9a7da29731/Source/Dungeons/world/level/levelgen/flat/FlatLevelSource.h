#pragma once

#include "common_header.h"

#include "world/level/chunk/ChunkSource.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"

class Block;
class Dimension;

class FlatLevelSource : public ChunkSource {
public:

	static const std::string DEFAULT_LAYERS;

	typedef std::vector<const Block*> LayerList;

	FlatLevelSource(Level* level, Dimension* dimension, const std::string& layersDesc);

	virtual void loadChunk(LevelChunk& c, bool forceImmediateReplacementDataLoad) override;

	virtual LevelChunk* requestChunk(const ChunkPos& cp, LoadMode lm) override;

	virtual bool postProcess(ChunkViewSource& chunks) override;

	Unique<CompoundTag> getStructureTag() const override;
	void readStructureTag(const CompoundTag& tag) override;

protected:

private:

	std::vector<BlockID> mPrototype;

	const PerlinSimplexNoise noise;
};
