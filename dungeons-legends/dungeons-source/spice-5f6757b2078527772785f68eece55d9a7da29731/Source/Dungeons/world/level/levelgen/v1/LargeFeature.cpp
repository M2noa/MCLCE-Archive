#include "Dungeons.h"

#include "world/level/levelgen/v1/LargeFeature.h"
#include "world/level/Level.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/dimension/Dimension.h"

LargeFeature::LargeFeature() :
	radius(8){
}

LargeFeature::~LargeFeature(){
}

void LargeFeature::apply(LevelChunk& lc, Dimension& dimension, Random& random){
	int r = radius;

	random.setSeed(dimension.getBiomes().getSeed());
	int xScale = random.nextInt() / 2 * 2 + 1;
	int zScale = random.nextInt() / 2 * 2 + 1;

	auto& lcPosition = lc.getPosition();
	for (int x = lcPosition.x - r; x <= lcPosition.x + r; x++) {
		for (int z = lcPosition.z - r; z <= lcPosition.z + r; z++) {
			random.setSeed((x * xScale + z * zScale) ^ dimension.getBiomes().getSeed());
			addFeature(lc, dimension, random, x , z );
		}
	}
}
