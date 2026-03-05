#include "Dungeons.h"

#include "RandomScatteredLargeFeature.h"
#include "StructureFeature.h"
#include "StructureStart.h"
#include "ScatteredFeaturePieces.h"
#include "world/level/BlockSource.h"
#include "world/level/BlockPos.h"
#include "world/level/biome/Biome.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/dimension/Dimension.h"

ScatteredFeatureStart::ScatteredFeatureStart()
	: StructureStart(0, 0) {
	// for serialization
}

ScatteredFeatureStart::ScatteredFeatureStart(int x, int z) 
	: StructureStart(x, z) {
	// for reflection
}

ScatteredFeatureStart::ScatteredFeatureStart(Dimension& source, Random& random, int chunkX, int chunkZ)
	: StructureStart(chunkX, chunkZ) {

	Biome* biome = source.getBiomes().getBiome(BlockPos(chunkX * 16 + 8, 0, chunkZ * 16 + 8));
	if (biome != nullptr) {
		if (biome->mId == Biome::jungle->mId || biome->mId == Biome::jungleHills->mId) {
			pieces.emplace_back(make_unique<JunglePyramidPiece>(chunkX * 16, chunkZ * 16));
		}
		else if (biome->mId == Biome::swampland->mId || biome->mId == Biome::swamplandMutated->mId) {
			pieces.emplace_back(make_unique<SwamplandHut>(chunkX * 16, chunkZ * 16));
		}
		else if (biome->mId == Biome::desert->mId || biome->mId == Biome::desertHills->mId) {
			pieces.emplace_back(make_unique<DesertPyramidPiece>(chunkX * 16, chunkZ * 16));
		}
		else if (biome->mId == Biome::iceFlats->mId || biome->mId == Biome::taigaCold->mId) {
			pieces.emplace_back(make_unique<Igloo>(chunkX * 16, chunkZ * 16));
		}
	}

	calculateBoundingBox();
}

RandomScatteredLargeFeature::RandomScatteredLargeFeature() : StructureFeature() {
	allowedBiomes.push_back(Biome::desert->mId);
	allowedBiomes.push_back(Biome::desertHills->mId);
	allowedBiomes.push_back(Biome::jungle->mId);
	allowedBiomes.push_back(Biome::jungleHills->mId);
	allowedBiomes.push_back(Biome::swampland->mId);
	allowedBiomes.push_back(Biome::swamplandMutated->mId);
	allowedBiomes.push_back(Biome::iceFlats->mId);
	allowedBiomes.push_back(Biome::taigaCold->mId);

	mWitchHutEnemies.insert(mWitchHutEnemies.end(), MobSpawnerData(EntityType::Witch, 1, 1, 1, nullptr, nullptr, true));
}

std::string RandomScatteredLargeFeature::getFeatureName() {
	return "Temple";
}

bool RandomScatteredLargeFeature::isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& pos) {
	int xx = pos.x;
	int zz = pos.z;
	int x = pos.x;
	int z = pos.z;
	if (x < 0) {
		x -= mSpacing - 1;
	}
	if (z < 0) {
		z -= mSpacing - 1;
	}

	int xCenterFeatureChunk = x / mSpacing;
	int zCenterFeatureChunk = z / mSpacing;
	xCenterFeatureChunk *= mSpacing;
	zCenterFeatureChunk *= mSpacing;
	xCenterFeatureChunk += random.nextInt(mSpacing - mMinSeparation);
	zCenterFeatureChunk += random.nextInt(mSpacing - mMinSeparation);
	x = xx;
	z = zz;

	if ((x == xCenterFeatureChunk && z == zCenterFeatureChunk)) {
		Biome* biome = biomeSource->getBiome(BlockPos(x * 16 + 8, 0, z * 16 + 8));
		if (biome == nullptr) {
			return false;
		}

		for (auto it = allowedBiomes.begin(); it != allowedBiomes.end(); ++it) {
			if (biome->mId == *it) {
				return true;
			}
		}
	}

	return false;
}


Unique<StructureStart> RandomScatteredLargeFeature::createStructureStart(Dimension& generator, Random& random, const ChunkPos& cp) {
	return make_unique<ScatteredFeatureStart>(generator, random, cp.x, cp.z);
}

const MobList& RandomScatteredLargeFeature::getEnemiesAt(const BlockPos& pos) {
	StructureStart* structure= getStructureAt(pos.x, pos.y, pos.z);
	if (structure != nullptr) {
		for (auto& piece : structure->getPieces()) {
			if (piece->getType() == StructurePieceType::WitchHut) {
				return mWitchHutEnemies;
			}
		}
	}
	return mEmptyEnemies;
}
