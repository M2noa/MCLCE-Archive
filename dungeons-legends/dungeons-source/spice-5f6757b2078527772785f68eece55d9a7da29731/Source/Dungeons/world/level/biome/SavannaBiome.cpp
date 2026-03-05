#include "Dungeons.h"
#include "MobSpawnUtils.h"
#include "world/level/biome/SavannaBiome.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"
#include "world/level/block/DoublePlantBlock.h"
#include "world/level/levelgen/feature/DoublePlantFeature.h"
#include "world/level/biome/MutatedSavannaBiome.h"

SavannaBiome::SavannaBiome(int id) :
	Biome(id, Biome::BiomeType::Savanna) {
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Horse, 1, 2, 6, MobSpawnUtils::spawnHorseHerd));

	decorator->treeCount = 1;
	decorator->flowerCount = 4;
	decorator->grassCount = 20;
}

const FeaturePtr& SavannaBiome::getTreeFeature(Random* random){
	if(random->nextInt(5) > 0) {
		return decorator->savannaTreeFeature;
	}
	return decorator->oakFeature;
}

void SavannaBiome::decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit){
	if (!legacy) {
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		BlockPos pos = origin.offset(x, 0, z);
		pos.y = source->getHeightmap(pos.x, pos.z);
		decorator->doublePlantFeature->place(*source, pos, random, enum_cast(DoublePlantType::Grass));
	}
	Biome::decorate(source, random, origin, legacy, limit);
}

Unique<Biome> SavannaBiome::createMutatedCopy(int id) {
	auto mutatedBiome = make_unique<MutatedSavannaBiome>(id, this);

	mutatedBiome->mTemperature = (mTemperature + 1.0f) * .5f;
	mutatedBiome->mDepth = mDepth * .5f + .3f;
	mutatedBiome->mScale = mScale * .5f + 1.2f;

	return std::move(mutatedBiome);
}
