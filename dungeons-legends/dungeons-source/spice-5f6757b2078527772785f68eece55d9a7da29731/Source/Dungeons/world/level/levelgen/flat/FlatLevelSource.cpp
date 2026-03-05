#include "Dungeons.h"

#include "world/level/levelgen/flat/FlatLevelSource.h"

#include "world/level/chunk/LevelChunk.h"

//#include "platform/threading/ThreadLocal.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/block/Block.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/biome/Biome.h"
#include "world/level/storage/LevelStorage.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "nbt/CompoundTag.h"
#include "Core/Utility/static_vector.h"

const std::string FlatLevelSource::DEFAULT_LAYERS = "[7,3,3,2]";

FlatLevelSource::FlatLevelSource(Level* level, Dimension* dimension, const std::string& layersDesc) :
	ChunkSource(level, dimension, CHUNK_WIDTH)
	, noise(89328, 5){
	//try to get a flat world description and parse it
	Json::Value root;
	Json::Reader reader;

	const std::string TAG_NAME = "game_flatworldlayers";

	auto existingDesc = mLevel->getLevelStorage().loadData(TAG_NAME);

	if (existingDesc.empty()) {
		existingDesc = layersDesc;
	}

	if (!reader.parse(existingDesc, root)) {
		existingDesc = layersDesc;
		reader.parse(existingDesc, root);
	}

	std::vector<BlockID> column;

	//compose the column from JSON
	auto itr = root.begin();
	auto end = root.end();

	auto height = dimension->getHeight();

	for (int i = 0; i < height && itr != end; ++itr, ++i) {
		column.push_back((BlockID)(*itr).asInt());
	}

	mLevel->getLevelStorage().saveData(TAG_NAME, std::move(existingDesc));

	//copypaste this 256 times into a chunk
	mPrototype.resize(height * CHUNK_COLUMNS, BlockID::AIR);
	auto ptr = mPrototype.data();
	for(auto i : range(CHUNK_COLUMNS)) {
		UNUSED_VARIABLE(i);
		memcpy(ptr, column.data(), column.size());
		ptr += height;
	}
}

LevelChunk* FlatLevelSource::requestChunk(const ChunkPos& cp, LoadMode lm){
	return lm == LoadMode::None ? nullptr : new LevelChunk(*mLevel, *mDimension, cp);
}

void FlatLevelSource::loadChunk(LevelChunk& lc, bool forceImmediateReplacementDataLoad){
	lc.setAllBlockIDs(mPrototype, getDimension().getHeight());
	lc.recalcBlockLights();
	lc.recalcHeightmap();

	Random random(lc.getMin().hashCode());

	for (ChunkBlockPos p; p.x < CHUNK_WIDTH; ++p.x) {
		for (p.z = 0; p.z < CHUNK_WIDTH; ++p.z) {
			lc.setBiome(*Biome::plains, p);
		}
	}

	lc.setSaved();
// 	lc.changeState(ChunkState::Generating, ChunkState::Generated);
	lc.changeState(ChunkState::Generating, ChunkState::Loaded);
}

bool FlatLevelSource::postProcess(ChunkViewSource& neighborhood){
	// nothing to post-process, return true to advance to the next stage

	return true;
}

Unique<CompoundTag> FlatLevelSource::getStructureTag() const {
	return make_unique<CompoundTag>();
}

void FlatLevelSource::readStructureTag(const CompoundTag& tag) {
	UNUSED_PARAMETER(tag)
}
