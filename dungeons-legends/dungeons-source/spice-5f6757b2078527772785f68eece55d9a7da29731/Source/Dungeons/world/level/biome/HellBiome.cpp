#include "Dungeons.h"

#include "world/level/biome/HellBiome.h"
#include "world/entity/EntityTypes.h"
#include "BiomeDecorator.h"

HellBiome::HellBiome(int id) :
	Biome(id, Biome::BiomeType::Hell) {
	_enemies.clear();
	_friendlies.clear();
	_waterFriendlies.clear();
	_ambientFriendlies.clear();

	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Ghast, 50, 4, 4));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::PigZombie, 100, 4, 4));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::LavaSlime, 10, 1, 4));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::EnderMan, 6, 1, 1));
}

float HellBiome::getCreatureProbability() {
	return 0.3f;
}
