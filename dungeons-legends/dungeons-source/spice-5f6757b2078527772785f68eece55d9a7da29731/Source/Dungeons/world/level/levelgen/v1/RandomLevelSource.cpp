/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/v1/RandomLevelSource.h"

#include "world/level/levelgen/feature/Feature.h"

#include "world/level/levelgen/feature/CactusFeature.h"
#include "world/level/levelgen/feature/ClayFeature.h"
#include "world/level/levelgen/feature/FlowerFeature.h"
#include "world/level/levelgen/feature/TreeFeature.h"
#include "world/level/levelgen/feature/LakeFeature.h"
#include "world/level/levelgen/feature/OreFeature.h"
#include "world/level/levelgen/feature/ReedsFeature.h"
#include "world/level/levelgen/feature/SpringFeature.h"
#include "world/level/levelgen/feature/OakFeature.h"
#include "world/level/levelgen/structure/StructureFeatureType.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/ChunkPos.h"
#include "world/level/biome/Biome.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/material/Material.h"
#include "world/level/block/Block.h"
#include "world/level/block/HeavyBlock.h"
#include "world/level/Seasons.h"
#include "util/Random.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/levelgen/feature/PumpkinFeature.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"

#include "world/level/chunk/PostprocessingManager.h"
// #include "world/level/BlockTickingQueue.h"
// #include "util/PerfTimer.h"
#include "world/level/levelgen/v1/FarlandsFeature.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/block/BlockVolume.h"
#include "world/entity/EntityClassTree.h"
#include "nbt/CompoundTag.h"

namespace Gen {
	static const int CHUNK_HEIGHT = 8;
	static const int CHUNK_WIDTH = 4;
	static const int CHUNKCOUNT_XZ = 16 / CHUNK_WIDTH;
	static const int CHUNKCOUNT_Y = LEVEL_GEN_HEIGHT / CHUNK_HEIGHT;
	static const int NOISE_SIZE_X = CHUNKCOUNT_XZ + 1;
	static const int NOISE_SIZE_Y = LEVEL_GEN_HEIGHT / CHUNK_HEIGHT + 1;
	static const int NOISE_SIZE_Z = CHUNKCOUNT_XZ + 1;
}

const float RandomLevelSource::SNOW_CUTOFF = 0.5f;
const float RandomLevelSource::SNOW_SCALE = 0.3f;

RandomLevelSource::RandomLevelSource(Level* level, Dimension* dimension, RandomSeed seed, bool box) 
	: ChunkSource(level, dimension, CHUNK_WIDTH)
	//, Dimension(make_unique<BiomeSource>(seed, level->getLevelData().getGenerator()), 63)
	, mLevel(level)
	, strongholdFeature(&villageFeature)
	, scatteredFeature()
	, oceanMonumentFeature()
	, legacyDevice(box)
// 	, generatorHelpersPool([&] () {
// 	return make_unique<ThreadData>();
// }) 
{
	Random random(seed);

	minLimitPerlinNoise = make_unique<PerlinNoise>(random, 16);
	maxLimitPerlinNoise = make_unique<PerlinNoise>(random, 16);
	mainPerlinNoise = make_unique<PerlinNoise>(random, 8);
	surfaceNoise = make_unique<PerlinSimplexNoise>(random, 4);
	scaleNoise = make_unique<PerlinNoise>(random, 10);
	depthNoise = make_unique<PerlinNoise>(random, 16);
	forestNoise = make_unique<PerlinNoise>(random, 8);

	for(auto xb : range_incl(-2, 2)) {
		for(auto zb : range_incl(-2, 2)) {
			float weight = 10.0f / Math::sqrt(xb * xb + zb * zb + 0.2f);
			biomeWeights[xb + 2 + (zb + 2) * 5] = weight;
		}
	}
}

RandomLevelSource::~RandomLevelSource() {

}

/*public*/
void RandomLevelSource::prepareHeights(BlockVolume& blocks, int xOffs, int zOffs) {

// 	ScopedProfile("prepareHeights");
	float noiseBuffer[Gen::NOISE_SIZE_X * Gen::NOISE_SIZE_Y * Gen::NOISE_SIZE_Z];
	Biome* biomes[10 * 10];

	getDimension().getBiomes().fillRawBiomeData(biomes, xOffs * Gen::CHUNK_WIDTH - 2, zOffs * Gen::CHUNK_WIDTH - 2);
	getHeights(noiseBuffer, biomes, xOffs * Gen::CHUNKCOUNT_XZ, 0, zOffs * Gen::CHUNKCOUNT_XZ);

	auto levelHeightBits = (int)Math::log2((float)blocks.getHeight());

	for(auto xc : range(Gen::CHUNKCOUNT_XZ)) {
		int xCurr = xc * Gen::NOISE_SIZE_Z;
		int xNext = (xc + 1) * Gen::NOISE_SIZE_Z;

		for(auto zc : range(Gen::CHUNKCOUNT_XZ)) {
			int xzMinMin = (xCurr + zc) * Gen::NOISE_SIZE_Y;
			int xzMinMax = (xCurr + zc + 1) * Gen::NOISE_SIZE_Y;
			int xzMaxMin = (xNext + zc) * Gen::NOISE_SIZE_Y;
			int xzMaxMax = (xNext + zc + 1) * Gen::NOISE_SIZE_Y;

			for(auto yc : range(Gen::CHUNKCOUNT_Y)) {
				float yStep = 1 / (float)Gen::CHUNK_HEIGHT;
				float s0 = noiseBuffer[xzMinMin + yc];
				float s1 = noiseBuffer[xzMinMax + yc];
				float s2 = noiseBuffer[xzMaxMin + yc];
				float s3 = noiseBuffer[xzMaxMax + yc];

				float s0a = (noiseBuffer[xzMinMin + yc + 1] - s0) * yStep;
				float s1a = (noiseBuffer[xzMinMax + yc + 1] - s1) * yStep;
				float s2a = (noiseBuffer[xzMaxMin + yc + 1] - s2) * yStep;
				float s3a = (noiseBuffer[xzMaxMax + yc + 1] - s3) * yStep;

				for(auto y : range(Gen::CHUNK_HEIGHT)) {
					float xStep = 1 / (float)Gen::CHUNK_WIDTH;

					float _s0 = s0;
					float _s1 = s1;
					float _s0a = (s2 - s0) * xStep;
					float _s1a = (s3 - s1) * xStep;

					for(auto x : range(Gen::CHUNK_WIDTH)) {
						int offs = ((x + xc * Gen::CHUNK_WIDTH) << (levelHeightBits + 4)) | ((0 + zc * Gen::CHUNK_WIDTH) << levelHeightBits) | (yc * Gen::CHUNK_HEIGHT + y);
						offs -= blocks.getHeight();
						float zStep = 1 / (float)Gen::CHUNK_WIDTH;

						float val = _s0;
						float vala = (_s1 - _s0) * zStep;
						val -= vala;

						for (auto i : range(Gen::CHUNK_WIDTH)) {
							UNUSED_VARIABLE(i);
							if((val += vala) > 0) {
								blocks.setBlock(offs += blocks.getHeight(), Block::mStone->mID);
							}
							else if(yc * Gen::CHUNK_HEIGHT + y < getDimension().getSeaLevel()) {
								blocks.setBlock(offs += blocks.getHeight(), Block::mStillWater->mID);
							}
							else {
								blocks.setBlock(offs += blocks.getHeight(), BlockID::AIR);
							}
						}
						_s0 += _s0a;
						_s1 += _s1a;
					}

					s0 += s0a;
					s1 += s1a;
					s2 += s2a;
					s3 += s3a;
				}
			}
		}
	}
}

void RandomLevelSource::buildSurfaces(ThreadData& thread, BlockVolume& blocks, LevelChunk& levelChunk, const ChunkPos& chunkPos) {
// 	ScopedProfile("buildSurfaces");
	
	// y is zero (not used)
	Vec3 chunkBlockPos(chunkPos);
	surfaceNoise->getRegion(thread.depthBuffer, { chunkBlockPos.x, chunkBlockPos.z }, 16, 16, { 1.f / 8, 1.f / 8 }, 1.0f);

	ChunkBlockPos p;
	for(p.x = 0; p.x < 16; ++p.x) {
		for(p.z = 0; p.z < 16; ++p.z) {
			BlockPos t = p + levelChunk.getMin();
			levelChunk.getBiome(p).buildSurfaceAt(
				thread.random,
				blocks,
				t
			, 
				thread.depthBuffer[p.index2D()]
			, 
				getDimension().getSeaLevel()
			);
		}
	}
}

LevelChunk* RandomLevelSource::requestChunk(const ChunkPos& cp, LoadMode lm){
	return lm == LoadMode::None ? nullptr : new LevelChunk(*mLevel, *mDimension, cp);
}

void RandomLevelSource::loadChunk(LevelChunk& lc, bool forceImmediateReplacementDataLoad){
// 	auto& p = lc.getPosition();
// 	auto& thread = generatorHelpersPool.getLocal();
// 	auto& random = thread.random;
// 
// 	random.setSeed((RandomSeed)(p.x * 341872712l + p.z * 132899541l));	//@fix
// 
// 	memset(thread.blockBuffer.data(), 0, sizeof(thread.blockBuffer));
// 
// 	auto box = BlockVolume{ thread.blockBuffer, CHUNK_WIDTH, LEVEL_GEN_HEIGHT, CHUNK_WIDTH };
// 
// 	getDimension().getBiomes().fillBiomeData(&lc, lc.getMin().x, lc.getMin().z);
// 	prepareHeights(box, p.x, p.z);	//biomes, temperatures);
// 	buildSurfaces(thread, box, lc, p);
// 
// 	if (!legacyDevice) {
// 		ScopedProfile("caveFeatureApply");
// 		thread.caveFeature.apply(box, lc, getDimension(), random);
// 		//TODO make the underworlds COOL
// 		//thread.underworldFeature.apply(this, level, p.x, p.z, levelChunk->getRawBlocks(), CHUNK_BLOCK_COUNT);
// // 		if (thread.farlandsFeature) {
// // 			thread.farlandsFeature->apply(this, lc, getDimension(), random);
// // 		}
// 	}
// 
// 	{
// 		ScopedProfile("recalcHeightMap");
// 		lc.setAllBlockIDs(box.blocks, box.getHeight());
// 		lc.setAllBlockData(box.data, box.getHeight());
// 		lc.recalcHeightmap();
// 	}
// 
// 	if (lc.getGenerator() == nullptr) {
// 		lc._setGenerator(this);
// 	}
// 
// 	lc.changeState(ChunkState::Generating, ChunkState::Generated);
}

bool RandomLevelSource::postProcess( ChunkViewSource& neighborhood ) {
// 	//grab the center chunk
// 	auto& nmin = neighborhood.getArea().getBounds().mMin;
// 	ChunkPos centerPos( nmin.x + 1, nmin.z  + 1 );
// 
// 	auto lc = neighborhood.getExistingChunk( centerPos );
// 
// 	int xScale = 0;
// 	int zScale = 0;
// 	auto& lcPosition = lc->getPosition();
// 
// 	DEBUG_ASSERT(centerPos == lcPosition, "the chunk is in the wrong position, something's up mang");
// 	DEBUG_ASSERT(neighborhood.getArea().getBounds().mVolume == 9, "Wrong number of chunks in the PP neighborhood");
// 
// 	//the ownership is automatically released (RAII FTW)
// 	auto ownsNeighborhood = PostprocessingManager::instance.tryLock(lcPosition);
// 	if (ownsNeighborhood) {
// 		ScopedProfile("postProcessStart");
// 		//create a source allowed to read non-populated chunks
// 		BlockSource source(neighborhood.getLevel(), neighborhood.getDimension(), neighborhood, false, true);
// 
// 		//create a place where to store all the ticks
// 		BlockTickingQueue instaTickQueue(Tick(1));
// 		source.setTickingQueue(instaTickQueue);
// 
// 		Biome& biome = source.getBiome(lc->getMax());
// 		auto& random = generatorHelpersPool.getLocal().random;
// 		const int xo = lcPosition.x * 16;
// 		const int zo = lcPosition.z * 16;
// 		BlockPos origin(xo, 0, zo);
// 
// 		// Ensure that the seed is consistent for lake placement. This does mean that lakes can potentially be different if you regen  
// 		// an old level with the same seed.  
// 		random.setSeed(mLevel->getSeed());
// 		xScale = random.nextInt() / 2 * 2 + 1;
// 		zScale = random.nextInt() / 2 * 2 + 1;
// 		random.setSeed(((lcPosition.x * xScale) + (lcPosition.z * zScale)) ^ mLevel->getSeed());
// 			
// 		//place less lakes on the surface on older devices
// 		bool hasLake = false;
// 		if(biome != Biome::desert && biome != Biome::desertHills) {
// 			ScopedProfile("tryPlaceWaterLake");
// 			if(/*!hasVillage && */ random.nextInt(4) == 0) {
// 				int x = random.nextInt(3, 12);
// 				int y = random.nextInt(LEVEL_GEN_HEIGHT);
// 				int z = random.nextInt(3, 12);
// 
// 				if(!(y < 60 && legacyDevice)) {	//don't place underground lakes old old stuffs
// 					BlockPos featurePlacement = origin.offset(x, y, z);
// 					LakeFeature lakeFeature(Block::mStillWater->mID);
// 					lakeFeature.place(source, featurePlacement, random);
// 					hasLake = true;
// 				}
// 			}
// 		}
// 
// 		if(/*!hasVillage && */ !hasLake && !legacyDevice && random.nextInt(8) == 0) {
// 			ScopedProfile("tryPlaceLavaLake");
// 			int x = random.nextInt(3, 12);
// 			int yparam = random.nextInt(LEVEL_GEN_HEIGHT - 8) + 8;
// 			int y = random.nextInt(yparam);
// 			int z = random.nextInt(3, 12);
// 			if(y >= 60 && (y < 64 || random.nextInt(10) == 0)) {
// 				BlockPos featurePlacement = origin.offset(x, y, z);
// 				LakeFeature lakeFeature(Block::mStillLava->mID);
// 				lakeFeature.place(source, featurePlacement, random);
// 			}
// 		}
// 
// 		bool forceSave = false;
// 
// 		//decorate
// 		{
// 			ScopedProfile("decorate");
// 
// 			{
// 				ScopedProfile("villageFeature");
// 				villageFeature.apply(*lc, getDimension(), random);
// 			}
// 
// 			if(!legacyDevice) {
// 
// 				{
// 					ScopedProfile("mineshaftFeature");
// 					mineshaftFeature.apply(*lc, getDimension(), random);
// 				}
// 				{
// 					ScopedProfile("strongfoldFeature");
// 					strongholdFeature.apply(*lc, getDimension(), random);
// 				}
// 				{
// 					ScopedProfile("scatteredFeature");
// 					scatteredFeature.apply(*lc, getDimension(), random);
// 				}
// 				{
// 					ScopedProfile("oceanMonumentFeature");
// 					oceanMonumentFeature.apply(*lc, getDimension(), random);
// 				}
// 			}
// 
// 			random.setSeed(mLevel->getSeed());
// 			xScale = random.nextInt() / 2 * 2 + 1;
// 			zScale = random.nextInt() / 2 * 2 + 1;
// 			random.setSeed(((lcPosition.x * xScale) + (lcPosition.z * zScale)) ^ mLevel->getSeed());
// 			{
// 				ScopedProfile("villagePostProcess");
// 				forceSave |= villageFeature.postProcess(&source, random, lcPosition.x, lcPosition.z);
// 			}
// 
// 			if(!legacyDevice) {
// 				{
// 					ScopedProfile("mineshaftPostProcess");
// 					forceSave |= mineshaftFeature.postProcess(&source, random, lcPosition.x, lcPosition.z);
// 				}
// 				{
// 					forceSave |= strongholdFeature.postProcess(&source, random, lcPosition.x, lcPosition.z);
// 				}
// 				{
// 					forceSave |= scatteredFeature.postProcess(&source, random, lcPosition.x, lcPosition.z);
// 				}
// 				{
// 					forceSave |= oceanMonumentFeature.postProcess(&source, random, lcPosition.x, lcPosition.z);
// 				}
// 
// 				{
// 					ScopedProfile("monsterRoomFeature");
// 
// 					for (auto i : range(8)) {
// 						UNUSED_VARIABLE(i);
// 						int x = xo + random.nextInt(CHUNK_WIDTH) + (CHUNK_WIDTH / 2);
// 						int y = random.nextInt(LEVEL_GEN_HEIGHT);
// 						int z = zo + random.nextInt(CHUNK_WIDTH) + (CHUNK_WIDTH / 2);
// 						monsterRoomFeature.place(source, BlockPos(x, y, z), random);
// 					}
// 				}
// 			}
// 
// 		}
// 
// 		{
// 			ScopedProfile("freezeFrostProcess");
// 
// 			for(auto x : range(CHUNK_WIDTH)) {
// 				for(auto z : range(CHUNK_WIDTH)) {
// 					const BlockPos topPos = source.getTopRainBlockPos(origin.offset(x, 0, z));
// 					const BlockPos belowPos = topPos.below();
// 
// 					if(source.shouldFreezeIgnoreNeighbors(belowPos)) {
// 						source.setBlockAndData(belowPos, Block::mIce->mID, Block::UPDATE_CLIENTS);
// 					}
// 				}
// 			}
// 		}
// 
// 		{
// 			random.setSeed(((lcPosition.x * xScale) + (lcPosition.z * zScale)) ^ mLevel->getSeed());
// 			{
// 				ScopedProfile("biomeDecorate");
// 				biome.decorate(&source, random, lc->getMin(), legacyDevice, FLT_MAX);
// 			}
// 		}
// 
// 		{
// 			ScopedProfile("fixWater");
// 			_fixWaterAlongEdges(*lc, source, instaTickQueue);
// 		}
// 
// 		{
// 			ScopedProfile("postProcessSeasons");
// 			if (lc->checkSeasonsPostProcessDirty()) {
// 
// 				// applySeasonsPostProcess() needs the heightmap or we get snow inside structures.
// 				// we are going to call recalcHeightmap() right before calling applySeasonsPostProcess().
// 				//
// 				// Unfortunatly we still need to call updateLightsAndHeights() afterwards because
// 				// applySeasonsPostProcess() adds top snow blocks that need proper lighting.
// 				lc->recalcHeightmap();
// 				lc->applySeasonsPostProcess(source);
// 			}
// 		}
// 
// 		{
// 			ScopedProfile("instaTicks");
// 			instaTickQueue.tickAllPendingTicks(source);
// 		}
// 
// 		{
// 			ScopedProfile("postProcessLightsHeights");
// 			lc->updateLightsAndHeights(source);
// 		}
// 
// 		if (forceSave) {
// 			lc->setUnsaved();
// 		}
// 
// 		return true;
// 	}
	return false;
}

/*private*/
void RandomLevelSource::getHeights(float* noiseBuffer, Biome** biomes, int x, int y, int z) {

// 	ScopedProfile("getHeights");
	const float coordinateScale = 1 * 684.412f;
	const float heightScale = 1 * 684.412f;
	const float upperLimitScale = 512.0f;
	const float lowerLimitScale = 512.0f * 0.5f;

	const int noiseSize2d = Gen::NOISE_SIZE_X * Gen::NOISE_SIZE_Y;
	Vec2 noisePos2D((float)x, (float)z);

	float depthRegion[noiseSize2d];
	depthNoise->getRegion(depthRegion, noisePos2D, Gen::NOISE_SIZE_X, Gen::NOISE_SIZE_Z, Vec2(200.0, 200.0), 0.5);

	const int noiseSize3d = Gen::NOISE_SIZE_X * Gen::NOISE_SIZE_Y * Gen::NOISE_SIZE_Z;
	Vec3 noisePos((float)x, (float)y, (float)z);

	float mainNoiseRegion[noiseSize3d];
	mainPerlinNoise->getRegion(mainNoiseRegion, noisePos, Gen::NOISE_SIZE_X, Gen::NOISE_SIZE_Y, Gen::NOISE_SIZE_Z, Vec3(coordinateScale / 80.0f, heightScale / 160.0f, coordinateScale / 80.0f));

	float minLimitRegion[noiseSize3d];
	minLimitPerlinNoise->getRegion(minLimitRegion, noisePos, Gen::NOISE_SIZE_X, Gen::NOISE_SIZE_Y, Gen::NOISE_SIZE_Z, Vec3(coordinateScale, heightScale * 1.25f, coordinateScale));

	float maxLimitRegion[noiseSize3d];
	maxLimitPerlinNoise->getRegion(maxLimitRegion, noisePos, Gen::NOISE_SIZE_X, Gen::NOISE_SIZE_Y, Gen::NOISE_SIZE_Z, Vec3(coordinateScale, heightScale, coordinateScale));

	x = z = 0;

	int noiseBufIndex = 0;
	int pp = 0;

	const float baseSize = 17.0f / 2.0f;
	
	for(auto xx : range(Gen::NOISE_SIZE_X)) {
		for(auto zz : range(Gen::NOISE_SIZE_Z)) {
			float scaleSum = 0;
			float depthSum = 0;
			float weightSum = 0;

			int rr = 1;

			// [RH] Perform a 5x5 approximately-gaussian smoothing kernel in order to make biome
			//      height transitions less abrupt
			Biome* middleBiome = biomes[(xx + 2) + (zz + 2) * (Gen::NOISE_SIZE_X + 5)];

			for(auto xb : range_incl(-rr, rr)) {
				for(auto zb : range_incl(-rr, rr)) {
					Biome* b = biomes[(xx + xb + 2) + (zz + zb + 2) * (Gen::NOISE_SIZE_X + 5)];
					float depth = b->mDepth;
					float scale = b->mScale;
					// We should never have amplified biomes in Pocket Edition
//                  if(generator == LevelType.amplified && depth > 0) {
//                      depth = 1f + depth * 2;
//                      scale = 1 + scale * 4f;
//                  }
					float weight = biomeWeights[xb + 2 + (zb + 2) * 5] / (depth + 2);
					if(b->mDepth > middleBiome->mDepth) {
						weight /= 2;
					}
					scaleSum += scale * weight;
					depthSum += depth * weight;
					weightSum += weight;
				}
			}
			scaleSum /= weightSum;
			depthSum /= weightSum;

			scaleSum = scaleSum * 0.9f + 0.1f;
			depthSum = (depthSum * 4 - 1) / 8.0f;

			float rdepth = (depthRegion[pp] / 8000.0f);
			if(rdepth < 0) {
				rdepth = -rdepth * 0.3f;
			}
			rdepth = rdepth * 3.0f - 2.0f;

			if(rdepth < 0) {
				rdepth /= 2;
				if(rdepth < -1) {
					rdepth = -1;
				}
				rdepth /= 1.4f;
				rdepth /= 2;
			}
			else {
				if(rdepth > 1) {
					rdepth = 1;
				}
				rdepth /= 8;
			}

			pp++;

			float depth = depthSum;
			float scale = scaleSum;
			depth += rdepth * 0.2f;
			depth = depth * baseSize / 8.0f;

			float yCenter = baseSize + depth * 4;

			for(auto yy : range(Gen::NOISE_SIZE_Y)) {
				float yOffs = ((yy - (yCenter)) * 12 * 128 / LEVEL_GEN_HEIGHT) / scale;

				if(yOffs < 0) {
					yOffs *= 4;
				}

				float blendMin = minLimitRegion[noiseBufIndex] / lowerLimitScale;
				float blendMax = maxLimitRegion[noiseBufIndex] / upperLimitScale;

				float blendFactor = (mainNoiseRegion[noiseBufIndex] / 10 + 1) / 2;
				float val = Math::clampedLerp(blendMin, blendMax, blendFactor) - yOffs;

				// [RH] If we're in the 4 noise segments, decrease 'val' somewhat sharply in order
				//      to ensure that there aren't truncated mountain peaks at the top of the terrain.
				if(yy > Gen::NOISE_SIZE_Y - 4) {
					float slide = (yy - (Gen::NOISE_SIZE_Y - 4)) / (4 - 1.0f);
					val = val * (1 - slide) + -10 * slide;
				}

				noiseBuffer[noiseBufIndex] = val;
				noiseBufIndex++;
			}
		}
	}
}

void RandomLevelSource::postProcessMobsAt(BlockSource* blockSource, int chunkWestBlock, int chunkNorthBlock, Random& random){
	villageFeature.postProcessMobsAt(blockSource, chunkWestBlock, chunkNorthBlock, random);

	scatteredFeature.postProcessMobsAt(blockSource, chunkWestBlock, chunkNorthBlock, random);

	mineshaftFeature.postProcessMobsAt(blockSource, chunkWestBlock, chunkNorthBlock, random);

	oceanMonumentFeature.postProcessMobsAt(blockSource, chunkWestBlock, chunkNorthBlock, random);
}

std::string RandomLevelSource::gatherStats() {
	return "RandomLevelSource";
}

const MobList& RandomLevelSource::getMobsAt(BlockSource& blockSource, EntityType category, const BlockPos& pos) {

	if (EntityClassTree::isTypeInstanceOf(category, EntityType::Monster)) {
		if (oceanMonumentFeature.isInsideFeature(pos.x, pos.y, pos.z)) {
			return oceanMonumentFeature.getEnemies();
		}

		const MobList& mobList = scatteredFeature.getEnemiesAt(pos);
		if (mobList.size() > 0) {
			return mobList;
		}
	}

	return ChunkSource::getMobsAt(blockSource, category, pos);
}

bool RandomLevelSource::findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) {
	bool result = false;

	switch (feature) {
		case StructureFeatureType::Mineshaft: {
			result = mineshaftFeature.getNearestGeneratedFeature(player, pos);
			break;
		}
		case StructureFeatureType::Monument: {
			result = oceanMonumentFeature.getNearestGeneratedFeature(player, pos);
			break;
		}
		case StructureFeatureType::Stronghold: {
			result = strongholdFeature.getNearestGeneratedFeature(player, pos);
			break;
		}
		case StructureFeatureType::Temple: {
			result = scatteredFeature.getNearestGeneratedFeature(player, pos);
		}
		case StructureFeatureType::Village: {
			result = villageFeature.getNearestGeneratedFeature(player, pos);
			break;
		}
		default: {
			break;
		}
	}

	return result;
}

void RandomLevelSource::_fixWaterAlongEdges(LevelChunk& lc, BlockSource& source, BlockTickingQueue& instaTickQueue){
	static const BlockPos offsets[] = { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 0, 2 }, { 0, 0, 3 }, { 0, 0, 4 }, { 0, 0, 5 }, { 0, 0, 6 }, { 0, 0, 7 }, { 0, 0, 8 }, { 0, 0, 9 }, { 0, 0, 10 }, { 0, 0, 11 }, { 0, 0, 12 }, { 0, 0, 13 }, { 0, 0, 14 }, { 0, 0, 15 }, { 1, 0, 0 }, { 1, 0, 15 }, { 2, 0, 0 }, { 2, 0, 15 }, { 3, 0, 0 }, { 3, 0, 15 }, { 4, 0, 0 }, { 4, 0, 15 }, { 5, 0, 0 }, { 5, 0, 15 }, { 6, 0, 0 }, { 6, 0, 15 }, { 7, 0, 0 }, { 7, 0, 15 }, { 8, 0, 0 }, { 8, 0, 15 }, { 9, 0, 0 }, { 9, 0, 15 }, { 10, 0, 0 }, { 10, 0, 15 }, { 11, 0, 0 }, { 11, 0, 15 }, { 12, 0, 0 }, { 12, 0, 15 }, { 13, 0, 0 }, { 13, 0, 15 }, { 14, 0, 0 }, { 14, 0, 15 }, { 15, 0, 0 }, { 15, 0, 1 }, { 15, 0, 2 }, { 15, 0, 3 }, { 15, 0, 4 }, { 15, 0, 5 }, { 15, 0, 6 }, { 15, 0, 7 }, { 15, 0, 8 }, { 15, 0, 9 }, { 15, 0, 10 }, { 15, 0, 11 }, { 15, 0, 12 }, { 15, 0, 13 }, { 15, 0, 14 }, { 15, 0, 15 } };

	for (auto& offset : offsets) {
		BlockPos p = lc.getMin() + offset;
		bool wasAir = true;

		auto h = source.getHeightmap(p);

		for (; p.y < h; ++p.y) {
			if (source.getBlockID(p) == Block::mStillWater->mID) {
				if (wasAir) {
					source.setBlockAndData(p, Block::mFlowingWater->mID, 0);
// 					instaTickQueue.add(source, p, Block::mFlowingWater->mID, 1);
					wasAir = false;
				}
			}
			else{
				wasAir = true;
			}
		}
	}
}

Unique<CompoundTag> RandomLevelSource::getStructureTag() const {
	Unique<CompoundTag> structures = make_unique<CompoundTag>();
	structures->putCompound("village", villageFeature.getStructureTag());
	structures->putCompound("stronghold", strongholdFeature.getStructureTag());
	structures->putCompound("scattered", scatteredFeature.getStructureTag());
	structures->putCompound("mineshaft", mineshaftFeature.getStructureTag());
	structures->putCompound("oceans", oceanMonumentFeature.getStructureTag());
	return structures;
}

void RandomLevelSource::readStructureTag(const CompoundTag& tag) {
	const CompoundTag* compoundTag = tag.getCompound("village");
	if (compoundTag) {
		villageFeature.readStructureTag(*compoundTag, getLevel());
	}
	compoundTag = tag.getCompound("stronghold");
	if (compoundTag) {
		strongholdFeature.readStructureTag(*compoundTag, getLevel());
	}
	compoundTag = tag.getCompound("scattered");
	if (compoundTag) {
		scatteredFeature.readStructureTag(*compoundTag, getLevel());
	}
	compoundTag = tag.getCompound("mineshaft");
	if (compoundTag) {
		mineshaftFeature.readStructureTag(*compoundTag, getLevel());
	}
	compoundTag = tag.getCompound("oceans");
	if (compoundTag) {
		oceanMonumentFeature.readStructureTag(*compoundTag, getLevel());
	}
}
