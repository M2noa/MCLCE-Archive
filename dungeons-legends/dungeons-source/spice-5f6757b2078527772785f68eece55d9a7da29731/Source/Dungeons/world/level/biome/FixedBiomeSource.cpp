#include "Dungeons.h"

#include "world/level/biome/FixedBiomeSource.h"
#include "world/level/chunk/LevelChunk.h"

FixedBiomeSource::FixedBiomeSource(RandomSeed seed, int biomeId) :
	BiomeSource(seed)
	, biomeId(biomeId){
}

void FixedBiomeSource::fillBiomeData(LevelChunk* levelChunk, int x, int z) const {
	for (char zOff = 0; zOff < CHUNK_WIDTH; zOff++) {
		for (char xOff = 0; xOff < CHUNK_WIDTH; xOff++) {
			const ChunkBlockPos ctp(xOff, 0, zOff);
			levelChunk->setBiome(*Biome::getBiome(biomeId), ctp);
		}
	}
}

Biome* FixedBiomeSource::getBiome(int blockX, int blockZ) {
	return Biome::getBiome(biomeId);
}

Biome* FixedBiomeSource::getBiome(const BlockPos& block) {
	return Biome::getBiome(biomeId);
}
