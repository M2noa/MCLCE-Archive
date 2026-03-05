#include "Dungeons.h"

#include "world/level/levelgen/structure/EndCityFeature.h"

#include "world/level/biome/BiomeSource.h"
#include "world/level/block/BlockVolume.h"
#include "world/level/chunk/ChunkSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Level.h"
#include "world/level/levelgen/structure/EndCityPieces.h"
#include "world/level/levelgen/v1/TheEndRandomLevelSource.h"

EndCityFeature::EndCityFeature(TheEndRandomLevelSource& levelSource)
	: mLevelSource(levelSource) {

	EndCityPieces::init();
}

bool EndCityFeature::isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& pos) {
	const static int citySpacing = 20;
	const static int minCitySeparation = 11;

	ChunkPos cp(pos);
	if (cp.x < 0) {
		cp.x -= citySpacing - 1;
	}
	if (cp.z < 0) {
		cp.z -= citySpacing - 1;
	}

	int xCenterCityChunk = cp.x / citySpacing;
	int zCenterCityChunk = cp.z / citySpacing;

	int64_t seed = (xCenterCityChunk * 341873128712l + zCenterCityChunk * 132897987541l) + biomeSource->getSeed() + 10387313;
	random.setSeed(RandomSeed(seed & 0xffffffff));

	xCenterCityChunk *= citySpacing;
	zCenterCityChunk *= citySpacing;

	int rand1 = random.nextInt(citySpacing - minCitySeparation);
	int rand2 = random.nextInt(citySpacing - minCitySeparation);
	xCenterCityChunk += (rand1 + rand2) / 2;

	rand1 = random.nextInt(citySpacing - minCitySeparation);
	rand2 = random.nextInt(citySpacing - minCitySeparation);
	zCenterCityChunk += (rand1 + rand2) / 2;

	cp = pos;
	if (cp.x == xCenterCityChunk && cp.z == zCenterCityChunk) {
		if (mLevelSource.isIslandChunk(cp.x, cp.z)) {
			return true;
		}
	}

	return false;
}

Unique<StructureStart> EndCityFeature::createStructureStart(Dimension& dimension, Random& random, const ChunkPos& pos) {
	return std::make_unique<EndCityStart>(dimension, mLevelSource, random, pos);
}

EndCityStart::EndCityStart()
	: StructureStart(0, 0) {
	// for serialization
}

EndCityStart::EndCityStart(Dimension& dimension, TheEndRandomLevelSource& levelSource, Random& random, const ChunkPos& pos)
	: StructureStart(pos.x, pos.z) {

	_create(dimension, levelSource, random, pos);
}

void EndCityStart::_create(Dimension& dimension, TheEndRandomLevelSource& levelSource, Random& random, const ChunkPos& pos) {
	// Temp LevelChunk used to calculate valid positions, the actual
	// chunk is loaded as normal in TheEndRandomLevelSource
	LevelChunk lc(dimension.getLevel(), dimension, pos);
	std::array<BlockID, CHUNK_WIDTH * CHUNK_WIDTH * TheEndGen::GEN_DEPTH> blockBuffer;
	BlockVolume box(blockBuffer, CHUNK_WIDTH, TheEndGen::GEN_DEPTH, CHUNK_WIDTH);
	levelSource.prepareHeights(box, pos.x, pos.z);
	lc.setAllBlockIDs(blockBuffer, box.getHeight());

	// Determine if there is a valid island to set the city at
	int offsetX = 5;
	int offsetZ = 5;

	Rotation rotation = Rotation(random.nextInt(4));
	switch (rotation) {
	case Rotation::ROTATE_90:
		offsetX = -5;
		break;
	case Rotation::ROTATE_180:
		offsetX = -5;
		offsetZ = -5;
		break;
	case Rotation::ROTATE_270:
		offsetZ = -5;
		break;
	default:
		break;
	}

	int height = dimension.getHeight() - 1;
	int y1 = lc.getAboveTopSolidBlock(ChunkBlockPos(7, height, 7), dimension.showSky());
	int y2 = lc.getAboveTopSolidBlock(ChunkBlockPos(7, height, 7 + offsetZ), dimension.showSky());
	int y3 = lc.getAboveTopSolidBlock(ChunkBlockPos(7 + offsetX, height, 7), dimension.showSky());
	int y4 = lc.getAboveTopSolidBlock(ChunkBlockPos(7 + offsetX, height, 7 + offsetZ), dimension.showSky());
	int y = Math::min(Math::min(y1, y2), Math::min(y3, y4));

	if (y < 60) {
		mIsValid = false;
		return;
	}

	// Construct city
	BlockPos offset(pos.x * 16 + TheEndGen::CHUNK_WIDTH, y, pos.z * 16 + TheEndGen::CHUNK_WIDTH);
	EndCityPieces::startHouseTower(dimension.getLevel().getStructureManager(), offset, rotation, pieces, random);

	calculateBoundingBox();

	mIsValid = true;
}
