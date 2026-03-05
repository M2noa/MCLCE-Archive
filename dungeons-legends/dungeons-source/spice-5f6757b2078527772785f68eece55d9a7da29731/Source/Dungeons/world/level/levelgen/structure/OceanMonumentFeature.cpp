/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/structure/OceanMonumentFeature.h"
#include "world/level/levelgen/structure/OceanMonumentPieces.h"
#include "world/level/biome/Biome.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/BlockSource.h"
#include "world/Facing.h"
#include "world/level/ChunkPos.h"

OceanMonumentFeature::OceanMonumentFeature() : StructureFeature() {
	allowedSpawnBiomes.push_back(Biome::deepOcean->mId);

	allowedBiomes.push_back(Biome::deepOcean->mId);
	allowedBiomes.push_back(Biome::frozenOcean->mId);
	allowedBiomes.push_back(Biome::frozenRiver->mId);
	allowedBiomes.push_back(Biome::ocean->mId);
	allowedBiomes.push_back(Biome::river->mId);

	mMonumentEnemies.insert(mMonumentEnemies.end(), MobSpawnerData(EntityType::Guardian, 1, 2, 4, nullptr, nullptr, false));
}

std::string OceanMonumentFeature::getFeatureName() {
	return "Monument";
}

bool OceanMonumentFeature::isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& pos) {
	ChunkPos cp(pos);
	if (cp.x < 0) {
		cp.x -= mMonumentSpacing - 1;
	}
	if (cp.z < 0) {
		cp.z -= mMonumentSpacing - 1;
	}

	int xCenterMonumentChunk = cp.x / mMonumentSpacing;
	int zCenterMonumentChunk = cp.z / mMonumentSpacing;

	int64_t seed = (xCenterMonumentChunk * 341873128712l + zCenterMonumentChunk * 132897987541l) + biomeSource->getSeed() + 10387313;
	random.setSeed((RandomSeed)(seed & 0xffffffff));

	xCenterMonumentChunk *= mMonumentSpacing;
	zCenterMonumentChunk *= mMonumentSpacing;

	int rand1 = random.nextInt(mMonumentSpacing - mMinMonumentSeparation);
	int rand2 = random.nextInt(mMonumentSpacing - mMinMonumentSeparation);
	xCenterMonumentChunk += (rand1 + rand2) / 2;

	rand1 = random.nextInt(mMonumentSpacing - mMinMonumentSeparation);
	rand2 = random.nextInt(mMonumentSpacing - mMinMonumentSeparation);
	zCenterMonumentChunk += (rand1 + rand2) / 2;

	cp = pos;
	if (cp.x == xCenterMonumentChunk && cp.z == zCenterMonumentChunk) {
		//center position must be in deep ocean
		if (!biomeSource->containsOnly(cp.x * 16 + 8, cp.z * 16 + 8, 16, allowedSpawnBiomes)) {
			return false;
		}
		//monument must be fully submerged in water
		int biomeRange = MonumentBuilding::getBiomeRange();
		bool biomeOk = biomeSource->containsOnly(cp.x * 16 + 8, cp.z * 16 + 8, biomeRange, allowedBiomes);
		if (biomeOk) {
			return true;
		}
	}
	return false;
}

Unique<StructureStart> OceanMonumentFeature::createStructureStart(Dimension& generator, Random& random, const ChunkPos& lc) {
	return make_unique<OceanMonumentStart>(generator, random, lc.x, lc.z);
}

OceanMonumentStart::OceanMonumentStart()
	: StructureStart(0, 0) {
	// for serialization
}

bool OceanMonumentStart::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (!isCreated) {
		pieces.clear();
		Dimension& dim = level->getDimension();
		createMonument(dim, random, getChunkX(), getChunkZ());
	}

	return StructureStart::postProcess(level, random, chunkBB);
}

void OceanMonumentStart::addAdditionalSaveData(CompoundTag& tag) {
	tag.putBoolean("iscreated", isCreated);
}

void OceanMonumentStart::readAdditionalSaveData(const CompoundTag& tag) {
	isCreated = tag.getBoolean("iscreated");
}

OceanMonumentStart::OceanMonumentStart(Dimension& dim, Random& random, int x, int z)
	: StructureStart(x, z) {
	createMonument(dim, random, x, z);
}

void OceanMonumentStart::createMonument(Dimension& dim, Random& random, int x, int z) {
	Level& level = dim.getLevel();
	random.setSeed(level.getSeed());
	float xScale = random.nextFloat();
	float zScale = random.nextFloat();
	float xx = x * xScale;
	float zz = z * zScale;
	random.setSeed(static_cast<RandomSeed>(xx) ^ static_cast<RandomSeed>(zz) ^ level.getSeed());
	
	int west = x * 16 + 8 - MonumentBuilding::getBiomeRange();
	int north = z * 16 + 8 - MonumentBuilding::getBiomeRange();
	int orientation = Facing::getRandomFaceHorizontal(random);
	
	pieces.emplace_back(make_unique<MonumentBuilding>(random, west, north, orientation));
	
	calculateBoundingBox();
	
	isCreated = true;
}
