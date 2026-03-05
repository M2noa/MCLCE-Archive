/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#include "Dungeons.h"
#include "world/level/biome/MobSpawnUtils.h"
#include "util/Random.h"
// #include "world/entity/animal/Horse.h"
#include "world/entity/EntityTypes.h"

void MobSpawnUtils::spawnHorseHerd(std::vector<Mob*>& mobHerd, Random& random) {
	if (mobHerd.empty()) {
		return;
	}

	// all horses in a herd are the same color
// 	int skin = random.nextInt((int)HorseVariant::VARIANT_COUNT);
// 
// 	for (auto& mob : mobHerd) {
// 		if (!mob || !mob->hasType(EntityType::Horse)) {
// 			continue;
// 		}
// 
// 		int mark = random.nextInt((int)HorseMarkings::MARKING_COUNT);
// 
// 		mob->setVariant(skin);
// 		mob->setMarkVariant(mark);
// 	}
}

void MobSpawnUtils::spawnPolarBearHerd(std::vector<Mob*>& mobHerd, Random& random) {
	if (mobHerd.size() < 2) {
		return;
	}

	// all polar bears after the first will be babies
// 	const size_t numBears = mobHerd.size();
// 	for (size_t i = 1; i < numBears; ++i) {
// 		Mob* mob = mobHerd[i];
// 		if (mob && !mob->isBaby()) {
// 			// remove adult definitions
// 			mob->removeDefinitionGroup("minecraft:adult");
// 			mob->removeDefinitionGroup("minecraft:adult_wild");
// 
// 			// add baby definitions
// 			mob->addDefinitionGroup("minecraft:baby");
// 			mob->addDefinitionGroup("minecraft:baby_wild");
// 		}
// 	}
}

EntityDefinitionIdentifier MobSpawnUtils::spawnZombie(Random& random) {
	if (random.nextInt(5) == 0) {
		return EntityDefinitionIdentifier(EntityType::ZombieVillager);
	}
	return EntityDefinitionIdentifier(EntityType::Zombie);
}