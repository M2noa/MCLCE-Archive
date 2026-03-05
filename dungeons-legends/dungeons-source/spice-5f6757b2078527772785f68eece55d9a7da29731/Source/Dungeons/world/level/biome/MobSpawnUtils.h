/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "CommonTypes.h"
#include "world/entity/EntityDefinitionIdentifier.h"

class Mob;
class Random;

class MobSpawnUtils {
public:
	static void spawnHorseHerd(std::vector<Mob*>& mobHerd, Random& random);
	static void spawnPolarBearHerd(std::vector<Mob*>& mobHerd, Random& random);
	static EntityDefinitionIdentifier spawnZombie(Random& random);
};
