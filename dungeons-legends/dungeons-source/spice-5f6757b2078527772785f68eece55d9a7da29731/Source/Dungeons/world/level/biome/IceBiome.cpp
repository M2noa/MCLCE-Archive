#include "Dungeons.h"
#include "world/level/biome/IceBiome.h"
#include "world/level/block/Block.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/biome/MobSpawnUtils.h"
#include "world/level/BlockSource.h"
#include "world/level/levelgen/feature/Feature.h"

IceBiome::IceBiome(int id, bool superIcy) :
	Biome(id, Biome::BiomeType::Ice)
	, superIcy(superIcy) {
	if(superIcy) {
		mTopMaterial = Block::mSnow->mID;
	}
	else {
		decorator->treeCount = 0.01f;
	}
	_friendlies.clear();

	for (size_t i = 0; i < _enemies.size(); ++i) {
		if (_enemies[i].mobClassId == EntityType::Skeleton) {
			_enemies.erase(_enemies.begin() + i);
			break;
		}
	}
	
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Stray, EntityDefinitionIdentifier("minecraft", EntityTypeToString(EntityType::Stray), "change_to_skeleton"), 120, 1, 2));

	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::PolarBear, 1, 1, 2, MobSpawnUtils::spawnPolarBearHerd));
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Rabbit, EntityDefinitionIdentifier("minecraft", "rabbit", "in_snow"), 4, 1, 2));
}

float IceBiome::getCreatureProbability() {
	return Biome::getCreatureProbability();
}

void IceBiome::decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit){
	if(superIcy) {
		for (auto i : range(3)) {
			UNUSED_VARIABLE(i);

			const int randX = origin.x + random.nextInt(16);
			const int randZ = origin.z + random.nextInt(16);
			BlockPos p = BlockPos(randX, 0, randZ);

			p.y = source->getHeightmap(p);

			decorator->iceSpikeFeature->place(*source, p, random);
		}

		for (auto i : range(2)) {
			UNUSED_VARIABLE(i);

			const int randX = origin.x + random.nextInt(16);
			const int randZ = origin.z + random.nextInt(16);
			BlockPos p = BlockPos(randX, 0, randZ);

			p.y = source->getHeightmap(p);

			decorator->icePatchFeature->place(*source, p, random);
		}
	}
	Biome::decorate(source, random, origin, legacy, limit);
}

const FeaturePtr& IceBiome::getTreeFeature(Random* random){
	return decorator->spruceTreeFeature;
}

Unique<Biome> IceBiome::createMutatedCopy(int id) {
	auto m = make_unique<IceBiome>(id, true);
	m->setColor(0xd2ffff, true);
	m->setName(mName + " Spikes");
	m->setSnowAccumulation(4.0f / 8.0f, 1.5f);
	m->setTemperatureAndDownfall(0, 1.0f);
	m->setDepthAndScale(BiomeHeight(mDepth + .1f, mScale + .1f));
	m->mDepth = mDepth + .3f;
	m->mScale = mScale + .4f;

	return std::move(m);
}
