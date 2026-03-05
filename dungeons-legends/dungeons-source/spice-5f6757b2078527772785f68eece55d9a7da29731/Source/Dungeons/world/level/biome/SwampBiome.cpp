#include "Dungeons.h"
#include "world/level/biome/SwampBiome.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/BlockPos.h"
#include "world/level/block/FlowerBlock.h"
#include "world/level/ChunkBlockPos.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/block/BlockVolume.h"

SwampBiome::SwampBiome(int id)
	: Biome(id, Biome::BiomeType::Swamp) {
	decorator->treeCount = 2;
	decorator->flowerCount = 1;
	decorator->deadBushCount = 1;
	decorator->mushroomCount = 8;
	decorator->reedsCount = 10;
	decorator->clayCount = 1;
	decorator->waterlilyCount = 4;
	decorator->sandCount = 0;
	decorator->gravelCount = 0;
	decorator->grassCount = 5;

	mWaterColor = Color::fromARGB(0x00c8af4b);
	mWaterColor.a = 0.8f;
	mUnderWaterColor = Color::fromARGB(0x001d1d21);
	mUnderWaterColor.a = 0.75f;

	// increase the chance of slimes in swamps
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Slime, 1, 1, 1));
}

const FeaturePtr& SwampBiome::getTreeFeature(Random* random){
	if (random->nextInt(20) == 0) {
		return decorator->hugeMushroomFeature;
	}
	else{
		return decorator->swampTreeFeature;
	}
}

int SwampBiome::getGrassColor( const BlockPos& pos ) {
	//jasonmaj $TODO Need to data drive this somehow. Ideally find spot in grass.png
	float groundValue = mBiomeInfoNoise->getValue(pos.x * 0.0225f, pos.z * 0.0225f);
	if(groundValue < -0.1f) {
		return 0x4c763c;
	}
	return 0x6a7039;
}

int SwampBiome::getMapGrassColor(const BlockPos& pos) {
	float groundValue = mBiomeInfoNoise->getValue(pos.x * 0.0225f, pos.z * 0.0225f);
	if (groundValue < -0.1f) {
		return 0x4c763c;
	}
	return 0x6a7039;
}

int SwampBiome::getFoliageColor() {
	return 0x6a7039;
}

int SwampBiome::getMapFoliageColor() {
	//jasonmaj $TODO Need to data drive this somehow. Ideally find spot in foliage.png
	return 0x6a7039;
}

FullBlock SwampBiome::getRandomFlowerTypeAndData(Random& random, const BlockPos& pos) {
	return FullBlock(Block::mRedFlower->mID, (DataID)FlowerBlock::ROSES::ORCHID);
}

void SwampBiome::buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) {
	float groundValue = mBiomeInfoNoise->getValue(pos.x * 0.25f, pos.z * 0.25f);
	if(groundValue > 0) {
		ChunkBlockPos p(pos);
		for(auto y : reverse_range(LEVEL_GEN_HEIGHT)) {
			p.y = y;
			auto block = blocks.block(p);
			auto& mat = Block::mBlocks[block]->getMaterial();

			if(mat.isType(MaterialType::Air)) {
				if(y == 62 && block != Block::mStillWater->mID) {
					blocks.setBlock(p, Block::mStillWater->mID);
					if(groundValue < 0.12f) {
						blocks.setBlock(p.above(), Block::mWaterlily->mID);
					}
				}
				break;
			}
		}
	}

	buildSurfaceAtDefault(random, blocks, pos, depthValue, seaLevel);
}
