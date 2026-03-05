#include "Dungeons.h"
#include "world/level/biome/JungleBiome.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"
#include "world/level/levelgen/feature/Feature.h"
#include "world/level/LevelConstants.h"
#include "world/entity/EntityTypes.h"

JungleBiome::JungleBiome(int id, bool isEdge) :
	Biome(id, Biome::BiomeType::Jungle)
	, isEdge(isEdge) {
	if(isEdge) {
		decorator->treeCount = 2;
	}
	else {
		decorator->treeCount = 25;
	}
	decorator->grassCount = 25;
	decorator->flowerCount = 4;

	if(!isEdge) {
		_friendlies.push_back(MobSpawnerData(EntityType::Ocelot, 2, 1, 1));
	}
	_friendlies.push_back( MobSpawnerData(EntityType::Chicken, 10, 4, 4));
}

const FeaturePtr& JungleBiome::getTreeFeature(Random* random){
	if(random->nextInt(10) == 0) {
		return decorator->fancyTreeFeature;
	}
	if(random->nextInt(2) == 0) {
		return decorator->jungleBushFeature;
	}
	if(!isEdge && random->nextInt(10) == 0) {
		return decorator->megaJungleTreeFeature;
	}
	return decorator->jungleTreeFeature;
}

const FeaturePtr& JungleBiome::getGrassFeature(Random* random){
	if(random->nextInt(4) == 0) {
		return decorator->fernFeature;
	}
	return decorator->tallGrassFeature;
}

void JungleBiome::decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit){
	Biome::decorate(source, random, origin, legacy, limit);
	{
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		BlockPos pos = origin.offset(x, 0, z);
		auto h = source->getHeightmap(pos.x, pos.z) * 2;
		pos.y = h ? random.nextInt(h) : 0;
		decorator->melonFeature->place(*source, pos, random);
	}

	for (auto i : range(50)) {
		UNUSED_VARIABLE(i);
		const int x = random.nextInt(16) + 8;
		const int y = LEVEL_GEN_HEIGHT / 2;
		const int z = random.nextInt(16) + 8;

		BlockPos pos = origin.offset(x, y, z);

		decorator->vinesFeature->place(*source, pos, random);
	}
}
