#pragma once

#include "common_header.h"

#include "world/entity/EntityTypes.h"
#include "util/WeighedRandom.h"

#include "world/entity/EntityDefinitionIdentifier.h"

class Mob;

class MobSpawnerData : public WeighedRandom::WeighedRandomItem {
public:

	EntityDefinitionIdentifier mIdentifier;
	EntityType mobClassId;
	int minCount;
	int maxCount;
	bool featureMob = false; //specifier for special mobs-eg nether fortress mobs

	// TODO trae - this should eventually be removed when biomes get data driven
	typedef std::function<void(std::vector<Mob*>&, Random&)> OnSpawnHerd;
	OnSpawnHerd mOnSpawnHerd = nullptr;

	typedef std::function<EntityDefinitionIdentifier(Random&)> OnSelectEntity;
	OnSelectEntity mOnSelectEntity = nullptr;

	MobSpawnerData() {
	}

	MobSpawnerData(EntityType mobClassId, int probabilityWeight, int minCount, int maxCount, OnSpawnHerd onSpawnHerd = nullptr, OnSelectEntity onSelectEntity = nullptr, bool featureMob = false)
		: WeighedRandom::WeighedRandomItem(probabilityWeight)
		, mobClassId(mobClassId)
		, minCount(minCount)
		, maxCount(maxCount)
		, mOnSpawnHerd(onSpawnHerd)
		, mOnSelectEntity(onSelectEntity)
		, featureMob(featureMob) {
	}

	MobSpawnerData(EntityType mobId, const std::string &fullDefinition, int probabilityWeight, int minCount, int maxCount)
		: WeighedRandom::WeighedRandomItem(probabilityWeight)
		, mIdentifier(fullDefinition)
		, mobClassId(mobId)
		, minCount(minCount)
		, maxCount(maxCount) {
	}

	MobSpawnerData(EntityType mobId, const EntityDefinitionIdentifier &ident, int probabilityWeight, int minCount, int maxCount)
		: WeighedRandom::WeighedRandomItem(probabilityWeight)
		, mIdentifier(ident)
		, mobClassId(mobId)
		, minCount(minCount)
		, maxCount(maxCount) {
	}
};
