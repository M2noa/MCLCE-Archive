#include "Dungeons.h"
#include "world/level/biome/MushroomIslandBiome.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/block/Block.h"
#include "world/entity/EntityTypes.h"

MushroomIslandBiome::MushroomIslandBiome(int id) :
	Biome(id, Biome::BiomeType::MushroomIsland) {
	decorator->treeCount = -100;
	decorator->flowerCount = -100;
	decorator->grassCount = -100;

	decorator->mushroomCount = 1;
	decorator->hugeMushrooms = 1;

	mTopMaterial = Block::mMycelium->mID;

	_enemies.clear();
	_friendlies.clear();
	_waterFriendlies.clear();
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::MushroomCow, 8, 4, 8));
	//friendlies.add(new MobSpawnerData(MushroomCow.class, 8, 4, 8));
}
