#include "Dungeons.h"

#include "world/level/biome/TheEndBiome.h"

#include "world/entity/EntityTypes.h"
#include "world/level/block/Block.h"
#include "world/level/biome/TheEndBiomeDecorator.h"

TheEndBiome::TheEndBiome(int id)
	: Biome(id, Biome::BiomeType::TheEnd, std::make_unique<TheEndBiomeDecorator>()) {

	_enemies.clear();
	_friendlies.clear();
	_waterFriendlies.clear();
	_ambientFriendlies.clear();

	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::EnderMan, 10, 4, 4));

	mTopMaterial = Block::mDirt->mID;
	mMaterial = Block::mDirt->mID;
}

Color TheEndBiome::getSkyColor(float temp) {
	return Color::BLACK;
}
