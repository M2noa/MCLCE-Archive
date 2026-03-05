#include "Dungeons.h"

#if 0

#include "world/level/levelgen/v1/FarlandsFeature.h"
#include "world/level/chunk/LevelChunk.h"
#include "util/Math.h"
#include "world/level/block/Block.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/biome/Biome.h"

FarlandsFeature::FarlandsFeature(RandomSeed seed, int limit, int endWidth, Height seaLevel) :
	noise(seed, 6, 2)
	, zoneNoise(seed + 1, 8)
	, limit(limit)
	, endWidth(endWidth)
	, seaLevel(seaLevel) {

	//find the max and min of the noise
	Random rand(seed);

	for (int i = 0; i < 10000; ++i) {
		const int randZ = rand.nextInt();
		const int randX = rand.nextInt();
		BlockPos pos(randX, 128, randZ);

		auto pn = noise.getValue(pos);
		max = std::max(pn, max);
		min = std::min(pn, min);
	}

}

#define BLOCK(P) (blocks[ P.y + (P.z - lc.getMin().z) * LEVEL_HEIGHT + (P.x - lc.getMin().x) * LEVEL_HEIGHT * CHUNK_DEPTH ])

void _checkDistances(BlockPos p, float& minDist, float& maxDist) {
	p.y = 0;
	float d = Vec3(p).length();
	minDist = std::min(minDist, d);
	maxDist = std::max(maxDist, d);
}

bool FarlandsFeature::isInsideLimit(const LevelChunk& lc, float margin) const {

	float limitMin = (float)limit;
	float minDist = FLT_MAX, maxDist = FLT_MIN;

	auto& lcMin = lc.getMin();
	auto& lcMax = lc.getMax();

	_checkDistances(lcMin, minDist, maxDist);
	_checkDistances(lcMax, minDist, maxDist);
	_checkDistances(BlockPos(lcMin.x, 0, lcMax.z), minDist, maxDist);
	_checkDistances(BlockPos(lcMax.x, 0, lcMin.z), minDist, maxDist);

	return maxDist < (limitMin - margin);
}

void FarlandsFeature::apply(buffer_span_mut<BlockID> blocks, LevelChunk& lc, Dimension& biomeSource, Random& random) {

	float limitMin = (float)limit;
	float limitMax = limitMin + (float)endWidth;

	float minDist = FLT_MAX, maxDist = FLT_MIN;

	auto& lcMin = lc.getMin();
	auto& lcMax = lc.getMax();

	_checkDistances(lcMin, minDist, maxDist);
	_checkDistances(lcMax, minDist, maxDist);
	_checkDistances(BlockPos(lcMin.x, 0, lcMax.z), minDist, maxDist);
	_checkDistances(BlockPos(lcMax.x, 0, lcMin.z), minDist, maxDist);

	//check the position in the ring
	if (maxDist > limitMin) {
		Vec3 center(lcMin.x + 8.f, 0, lcMin.z + 8.f);

		float d;
		BlockPos pos;

		for (pos.x = lcMin.x; pos.x <= lcMax.x; ++pos.x) {
			for (pos.z = lcMin.z; pos.z <= lcMax.z; ++pos.z) {

				float dist = Vec3(pos).length();

				float f = powf((dist - limitMin) / (limitMax - limitMin), 1.f);

				const float edgeFactor = std::max(0.f, (f - 0.8f)) / 0.2f;
				const int ledgeH = 85;

				//find the topmost block
				pos.y = LEVEL_HEIGHT - 1;
				auto col = &BLOCK(pos);

				bool ocean = lc.getBiome((ChunkBlockPos)pos).getBiomeType() == Biome::BiomeType::Ocean;
				float s = ocean ? 0.4f : 1.f;

				//find first non-air
				for (pos.y = LEVEL_HEIGHT - 1; pos.y > 0 && *col == 0; --pos.y, --col) {
				}

				for (; pos.y >= 0; --pos.y, --col) {
					Vec3 noisePos(pos.x * s, pos.y * 0.25f, pos.z * s);
					float pn = noise.getValue(noisePos);

					//normalize the noise
					pn = ((pn - min) / (max - min));

					d = 0;
					if (pos.y <= ledgeH) {
						d = 1.f - powf(pos.y / (float)ledgeH, 10.f);
					}

					if (pos.y > 40 && edgeFactor > 0.01f) {
						d += ((pos.y - 40) / (float)(40)) * edgeFactor;
					}

					float f1 = Math::lerp(-f, f, d);

					if (ocean) {
						f1 -= 0.4f * Math::lerp(0.4f, 1.f,  std::min(pos.y / (float)40, 1.f));
					}

					if (pn < f1 + 0.1f) {
						if (*col == Block::mStillWater->mID) {

							//damn, water is tricky
							//check how deep it is, if is sea stop digging
//                          int depth = 0;
//                          for (int depth = 0; depth < pos.y && depth < 5 && col[depth] == Block::calmWater->id;
// ++depth);
//
//                          if (depth >= 5)//skip
//                              pos.y = -1;
//                          else  //dig the whole column
							for (; pos.y >= 0; --pos.y, --col) {
								*col = BlockID::AIR;
							}
						}
						else{
							*col = BlockID::AIR;
						}
					}
					else if (ocean && (pos.y < seaLevel - 1) && (pn < f1 + 0.2f)) {
						*col = Block::mStone->mID;
					}
				}
			}
		}
	}

}

void FarlandsFeature::addFeature( LevelChunk& lc, Dimension& generator, Random& random, int x, int z ) {

}

#undef BLOCK
#endif