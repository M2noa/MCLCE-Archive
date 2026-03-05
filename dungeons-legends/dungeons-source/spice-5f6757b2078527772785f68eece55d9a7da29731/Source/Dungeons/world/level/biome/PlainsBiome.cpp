#include "Dungeons.h"

#include "MobSpawnUtils.h"
#include "world/level/biome/PlainsBiome.h"
#include "world/level/BlockSource.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/BlockPos.h"
#include "world/level/block/FlowerBlock.h"
#include "world/level/block/DoublePlantBlock.h"
#include "world/level/levelgen/feature/DoublePlantFeature.h"

PlainsBiome::PlainsBiome(int id) :
	Biome(id, Biome::BiomeType::Plain) {
	setTemperatureAndDownfall(0.8f, 0.4f);
	setDepthAndScale(HEIGHTS_LOWLANDS);
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Horse, 5, 2, 6, MobSpawnUtils::spawnHorseHerd));

	decorator->treeCount = 0.05f;
	decorator->flowerCount = 4;
	decorator->grassCount = 10;
}

FullBlock PlainsBiome::getRandomFlowerTypeAndData(Random& random, const BlockPos& pos) {
	FullBlock flower;
	float flowerNoise = mBiomeInfoNoise->getValue(pos.x / 200.0f, pos.z / 200.0f);
	if(flowerNoise < -.8) {
		DataID select = random.nextInt(4);
		flower.id = Block::mRedFlower->mID;
		flower.data = (DataID)FlowerBlock::ROSES::TULIP_RED + select;
	}
	else {
		if(random.nextInt(3) > 0) {
			int select = random.nextInt(3);
			flower.id = Block::mRedFlower->mID;
			if(select == 0) {
				flower.data = (DataID)FlowerBlock::ROSES::POPPY;
			}
			else if(select == 1) {
				flower.data = (DataID)FlowerBlock::ROSES::HOUSTONIA;
			}
			else {
				flower.data = (DataID)FlowerBlock::ROSES::OXEYE;
			}
		}
		else {
			flower.id = Block::mYellowFlower->mID;
			flower.data = 0;
		}
	}

	return flower;
}

void PlainsBiome::decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit){
	float flowerNoise = mBiomeInfoNoise->getValue((origin.x + 8) / 200.0f, (origin.z + 8) / 200.0f);
	if(flowerNoise < -.8) {
		decorator->flowerCount = 15;
		decorator->grassCount = 5;
	}
	else if( !legacy ) {
		decorator->flowerCount = 4;
		decorator->grassCount = 10;

		DataID grassID = enum_cast(DoublePlantType::Grass);
		for (auto i : range(7)) {
			UNUSED_VARIABLE(i);
			int x = random.nextInt(16);
			int z = random.nextInt(16);
			BlockPos offsetPos = origin.offset(x, 0, z);
			offsetPos.y = random.nextInt(source->getHeightmap(offsetPos.x, offsetPos.z) + 32);
			decorator->doublePlantFeature->place(*source, offsetPos, random, grassID);
		}
	}
	if(sunflowers) {
		DataID sunflowerID = enum_cast(DoublePlantType::Sunflower);
		for (auto i : range(10)) {
			UNUSED_VARIABLE(i);
			int x = random.nextInt(16);
			int z = random.nextInt(16);
			BlockPos offsetPos = origin.offset(x, 0, z);
			offsetPos.y = random.nextInt(source->getHeightmap(offsetPos.x, offsetPos.z) + 32);
			decorator->doublePlantFeature->place(*source, offsetPos, random, sunflowerID);
		}
	}
	Biome::decorate(source, random, origin, legacy, limit);
}

Unique<Biome> PlainsBiome::createMutatedCopy(int id) {
	auto sunflowerBiome = make_unique<PlainsBiome>(id);
	sunflowerBiome->setName("Sunflower Plains");
	sunflowerBiome->sunflowers = true;
	sunflowerBiome->setColor(0x8db360);
	sunflowerBiome->mDebugMapOddColor = 0xd9cb4a;
	return std::move(sunflowerBiome);
}

FeaturePtr& PlainsBiome::getTreeFeature(Random* random){
	if (random->nextInt(3) == 0) {
		return decorator->fancyTreeFeature;
	}
	return decorator->oakFeature;
}
