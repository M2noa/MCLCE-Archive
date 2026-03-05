#include "Dungeons.h"

#include "LovikaLevelSource.h"

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

inline int positive_modulo(int i, int n) {
	return (i % n + n) % n;
}

LovikaLevelSource::LovikaLevelSource(Level* level, Dimension* dimension, io::ObjectGroup* objGroup) :
	ChunkSource(level, dimension, CHUNK_WIDTH)
	, objGroup(objGroup)
{
	auto height = dimension->getHeight();
	mPrototype.resize(height * CHUNK_COLUMNS, BlockID::AIR);
	mPrototypeData.resize(height * CHUNK_COLUMNS / 2);

	for (auto& object : objGroup->objects) {
		auto& bounds = object.bounds;
		BlockPos& min = bounds.minInclusive;
		BlockPos& max = bounds.maxExclusive;

		BlockPos diff = max - min;
		int chunkSizeX = FMath::CeilToInt((float)diff.x / CHUNK_WIDTH) + 1;
		int chunkSizeZ = FMath::CeilToInt((float)diff.z / CHUNK_WIDTH) + 1;

		int modX = positive_modulo(min.x, CHUNK_WIDTH);
		int modZ = positive_modulo(min.z, CHUNK_WIDTH);
		int quantMinX = min.x - modX;
		int quantMinZ = min.z - modZ;

		for (int chunkX = 0; chunkX < chunkSizeX; chunkX++) {
			for (int chunkZ = 0; chunkZ < chunkSizeZ; chunkZ++) {
				BlockPos currentMin(quantMinX + chunkX * CHUNK_WIDTH, min.y, quantMinZ + chunkZ * CHUNK_WIDTH);
				LevelChunk* chunk = requestChunk(ChunkPos(currentMin), LoadMode::Deferred);

				int index = 0;
				int indexData = 0;
				for (int x = 0; x < CHUNK_WIDTH; x++) {
					for (int z = 0; z < CHUNK_DEPTH; z++) {
						for (int y = 0; y < height; y++) {
							FullBlock block = object.blocks->getBlock(chunkX * CHUNK_WIDTH + x - modX, y - min.y, chunkZ * CHUNK_WIDTH + z - modZ);

							if (block.id != BlockID::AIR) {
								int fish = (int)block.id;
								fish = 0;
							}

							chunk->setBlockAndData(ChunkBlockPos(x, y, z), block);

							//mPrototype[index] = block.id;
							//if (index % 2 == 0) {
							//	mPrototypeData[indexData].first = block.data;
							//}
							//else {
							//	mPrototypeData[indexData].second = block.data;
							//	indexData++;
							//}

							//index++;
						}
					}
				}
				
			}
		}
	}
}

LevelChunk* LovikaLevelSource::getExistingChunk(const ChunkPos& cp) {
	//retrieve a chunk from the area if any
	auto element = mChunkMap.find(cp);

	return element != mChunkMap.end()
		? element->second.get()
		: nullptr;
}

LevelChunk* LovikaLevelSource::requestChunk(const ChunkPos& cp, LoadMode lm){
	//return lm == LoadMode::None ? nullptr : new LevelChunk(*mLevel, *mDimension, cp);

	auto elem = mChunkMap.find(cp);
	if (elem == mChunkMap.end()) {
		ChunkPtr lc;
		if (lm != LoadMode::None) {
			lc = make_unique<LevelChunk>(*mLevel, *mDimension, cp);

			loadChunk(*lc, false);

			lc->setFinalized(LevelChunk::Finalization::Done);
		}
		else {
			return nullptr;
		}

		return (mChunkMap[cp] = ChunkRefCount(std::move(lc), 1)).get();
	}
	else {
		return elem->second.grab();
	}
}

void LovikaLevelSource::loadChunk(LevelChunk& lc, bool forceImmediateReplacementDataLoad){

// 	lc.setAllBlockIDs(mPrototype, getDimension().getHeight());
// 	lc.setAllBlockData(mPrototypeData, getDimension().getHeight());
// 	lc.recalcBlockLights();
// 	lc.recalcHeightmap();

	for (ChunkBlockPos p; p.x < CHUNK_WIDTH; ++p.x) {
		for (p.z = 0; p.z < CHUNK_DEPTH; ++p.z) {
			lc.setBiome(*Biome::plains, p);
		}
	}

	if (lc.getGenerator() == nullptr) {
		lc._setGenerator(this);
	}

	lc.setSaved();
	lc.tryChangeState(ChunkState::Unloaded, ChunkState::Loaded);
}

bool LovikaLevelSource::postProcess(ChunkViewSource& neighborhood){
	// nothing to post-process, return true to advance to the next stage

	return true;
}

Unique<CompoundTag> LovikaLevelSource::getStructureTag() const {
	return make_unique<CompoundTag>();
}

void LovikaLevelSource::readStructureTag(const CompoundTag& tag) {
	UNUSED_PARAMETER(tag)
}
