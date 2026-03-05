#include "Dungeons.h"

#include "world/level/biome/BiomeSource.h"
#include "world/level/biome/Biome.h"
#include "world/level/newbiome/Layer.h"
#include "world/level/chunk/LevelChunk.h"
//#include "util/PerfTimer.h"
//#include "platform/threading/WorkerPool.h"

BiomeSource::BiomeSource(RandomSeed seed) :
	seed(seed) {

}

BiomeSource::BiomeSource(RandomSeed seed, GeneratorType type) :
	seed(seed) {
	Layer::getDefaultLayers(seed, biomeLayer, zoomedLayer, type);
	playerValidSpawns = { {
		Biome::forest->mId,
		Biome::plains->mId,
		Biome::taiga->mId,
		Biome::taigaHills->mId,
		Biome::forestHills->mId,
		Biome::jungle->mId,
		Biome::jungleHills->mId
	} };
}

void BiomeSource::fillBiomeData(LevelChunk* levelChunk, int x, int z) const {
	//ScopedProfile("fillBiomeData");
	LayerData layerData;

	zoomedLayer->fillArea(layerData, x, z, CHUNK_WIDTH, CHUNK_WIDTH);

	for(char zOff = 0; zOff < CHUNK_WIDTH; zOff++) {
		for(char xOff = 0; xOff < CHUNK_WIDTH; xOff++) {
			const ChunkBlockPos ctp(xOff, 0, zOff);
			Biome* biome = Biome::getBiome(layerData.mParentArea[ctp.index2D()]);
			DEBUG_ASSERT(biome != nullptr, "This should never return a nullptr");
			levelChunk->setBiome(*biome, ctp);
		}
	}
}

void BiomeSource::fillRawBiomeData(Biome** biomes, int x, int z) const {
	//ScopedProfile("fillRawBiomeData");
	LayerData layerData;

	biomeLayer->fillArea(layerData, x, z, 10, 10);

	for(auto i : range(10 * 10)) {
		Biome* biome = Biome::getBiome(layerData.mParentArea[i], Biome::DEFAULT);
		DEBUG_ASSERT(biome != nullptr, "This should never return a nullptr");
		biomes[i] = biome;
	}
}

BlockPos BiomeSource::getSpawnPosition() const {
	int x = 0, z = 0;
	BlockPos spawnPosition;
	LayerData layerData;

	while(!findValidSpawnPosition(layerData, x, z, spawnPosition)) {
		x += 40;
	}
	return spawnPosition;
}

bool BiomeSource::isValidSpawn(int currentBiome) const {
	for(auto&& playerSpawnBiome : playerValidSpawns) {
		if(currentBiome == playerSpawnBiome) {
			return true;
		}
	}
	return false;
}

bool BiomeSource::findValidSpawnPosition(LayerData& layerData, int x, int z, BlockPos& spawnPosition) const {
	biomeLayer->fillArea(layerData, x / 4, z / 4, 10, 10);

	for(auto zo : range(10)) {
		for(auto xo : range(10)) {
			if(!isValidSpawn(layerData.mParentArea[xo + zo * 10])) {
				continue;
			}
			if(!isValidSpawn(layerData.mParentArea[(xo - 1) + zo * 10])) {
				continue;
			}
			if(!isValidSpawn(layerData.mParentArea[(xo + 1) + zo * 10])) {
				continue;
			}
			if(!isValidSpawn(layerData.mParentArea[xo + (zo - 1) * 10])) {
				continue;
			}
			if(!isValidSpawn(layerData.mParentArea[xo + (zo + 1) * 10])) {
				continue;
			}

			// Setting height to invalid position so Player
			// can determine the right height on respawn
			spawnPosition = BlockPos(x + xo * 4, LEVEL_SPAWN_HEIGHT, z + zo * 4);

			return true;
		}
	}
	return false;
}

//void BiomeSource::debugVisulizeMap(int x, int y, int w, int h, std::weak_ptr<std::vector<int> > data, std::weak_ptr<ThreadLocal<BiomeSource> > source) {
//
//	x -= w / 2;
//	y -= h / 2;
//
//	for(int yy = 0; yy < h; yy += 16) {
//		for(int xx = 0; xx < w; xx += 16) {
//			WorkerPool::getFor(WorkerRole::Streaming).queue([x, y, w, h, yy, xx, data, source] () {
//				if(auto dataWrite = data.lock()) {
//					if(auto biomeSourceRead = source.lock()) {
//						LayerData layerData;
//						biomeSourceRead->getLocal().getZoomedLayer()->fillArea(layerData, x + xx, y + yy, 16, 16);
//
//						for(auto yo : range(16)) {
//							for(auto xo : range(16)) {
//								int xp = xx + xo;
//								int yp = yy + yo;
//								if(xp >= 0 && yp >= 0 && xp < w && yp < h) {
//									auto biome = Biome::getBiome(layerData.mParentArea[xo + yo * 16], Biome::DEFAULT);
//
//									if((((x + xo + y + yo) / 2) & 1) == 0) {
//										(*dataWrite)[xp + yp * w] = biome->mDebugMapOddColor | 0xff000000;
//									}
//									else {
//										(*dataWrite)[xp + yp * w] = biome->mDebugMapColor | 0xff000000;
//									}
//								}
//							}
//						}
//					}
//				}
//				return true;
//			});
//		}
//	}
//}

bool BiomeSource::containsOnly(int x, int z, int r, const std::vector<int>& allowed){
	int x0 = ((x - r) >> 2);
	int z0 = ((z - r) >> 2);
	int x1 = ((x + r) >> 2);
	int z1 = ((z + r) >> 2);

	int w = x1 - x0 + 1;
	int h = z1 - z0 + 1;

	LayerData layerData;
	biomeLayer->fillArea(layerData, x0, z0, w, h);

	for (int i = 0; i < w * h; i++) {
		bool found = false;

		for (unsigned int j = 0; j < allowed.size(); j++) {
			if (layerData.mParentArea[i] == allowed[j]) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}
	}
	return true;
}

Biome* BiomeSource::getBiome(int blockX, int blockZ){
	LayerData layerData;
	zoomedLayer->fillArea(layerData, blockX, blockZ, 1, 1);

	return Biome::getBiome(layerData.mParentArea[0], Biome::DEFAULT);
}

Biome* BiomeSource::getBiome(const BlockPos& block) {
	return getBiome(block.x, block.z);
}

LayerPtr BiomeSource::getZoomedLayer() {
	return zoomedLayer;
}

//float* BiomeSource::getDownfallBlock( /*float* downfalls__,*/ int x, int z, int w, int h )
//{
//	//const int size = w * h;
//	//if (lenDownfalls < size) {
//	//	delete[] downfalls;
//	//	downfalls = new float[size];
//	//	lenDownfalls = size;
//	//}
//
//	downfalls = downfallMap->getRegion(downfalls, x, z, w, w, downfallScale, downfallScale, 0.5f);
//	return downfalls;
//}
