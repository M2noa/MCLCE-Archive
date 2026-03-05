#pragma once

#include "world/level/biome/Biome.h"
#include "world/entity/EntityTypes.h"

class OceanBiome : public Biome {
public:

	OceanBiome(int id) :
		Biome(id, Biome::BiomeType::Ocean) {
		_friendlies.clear();

		_waterFriendlies.insert(_waterFriendlies.end(), MobSpawnerData(EntityType::Squid, 10, 1, 2));
	}

	BiomeTempCategory getTemperatureCategory() const override {
		return Biome::BiomeTempCategory::OCEAN;
	}

	virtual float getCreatureProbability() override {
		return 0.1f;
	}

};
