/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/v1/HellRandomLevelSource.h"

#include "world/level/levelgen/feature/FlowerFeature.h"
#include "world/level/levelgen/feature/HellFireFeature.h"
#include "world/level/levelgen/feature/HellSpringFeature.h"
#include "world/level/levelgen/feature/GlowStoneFeature.h"
#include "world/level/levelgen/feature/OreFeature.h"
#include "world/level/levelgen/structure/StructureFeatureType.h"

#include "world/level/material/Material.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/chunk/PostprocessingManager.h"
#include "world/level/block/Block.h"
#include "world/level/block/BlockVolume.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/levelgen/synth/PerlinNoise.h"
#include "world/entity/EntityClassTree.h"
#include "world/level/FoliageColor.h"
#include "world/level/dimension/Dimension.h"

#include "nbt/CompoundTag.h"
// #include "util/PerfTimer.h"

HellRandomLevelSource::HellRandomLevelSource(Level* level, Dimension* dimension, RandomSeed seed)
	: ChunkSource(level, dimension, CHUNK_WIDTH)
// 	, generatorHelpersPool([&] () {
// 	return make_unique<ThreadData>();
// }) 
{
	Random random(seed);

	lperlinNoise1 = make_unique<PerlinNoise>(random, 16);
	lperlinNoise2 = make_unique<PerlinNoise>(random, 16);
	perlinNoise1 = make_unique<PerlinNoise>(random, 8);
	perlinNoise2 = make_unique<PerlinNoise>(random, 4);
	perlinNoise3 = make_unique<PerlinNoise>(random, 4);

	scaleNoise = make_unique<PerlinNoise>(random, 10);
	depthNoise = make_unique<PerlinNoise>(random, 16);
}

void HellRandomLevelSource::prepareHeights(BlockVolume& blocks, int xOffs, int zOffs) {

// 	ScopedProfile("prepareHeights");

	float noiseBuffer[HellGen::NOISE_SIZE_X * HellGen::NOISE_SIZE_Y * HellGen::NOISE_SIZE_Z];

	getHeights(noiseBuffer, xOffs * HellGen::CHUNKCOUNT_XZ, 0, zOffs * HellGen::CHUNKCOUNT_XZ);
	
	auto levelHeightBits = (int)Math::log2((float)blocks.getHeight());
	auto step = 1 << levelHeightBits;

	for (uint32_t xc = 0; xc < HellGen::CHUNKCOUNT_XZ; xc++) {
		for (uint32_t zc = 0; zc < HellGen::CHUNKCOUNT_XZ; zc++) {
			for (uint32_t yc = 0; yc < blocks.getHeight() / HellGen::CHUNK_HEIGHT; yc++) {
				float yStep = 1 / (float)HellGen::CHUNK_HEIGHT;
				float s0 = noiseBuffer[((xc + 0) * HellGen::NOISE_SIZE_Z + (zc + 0)) * HellGen::NOISE_SIZE_Y + (yc + 0)];
				float s1 = noiseBuffer[((xc + 0) * HellGen::NOISE_SIZE_Z + (zc + 1)) * HellGen::NOISE_SIZE_Y + (yc + 0)];
				float s2 = noiseBuffer[((xc + 1) * HellGen::NOISE_SIZE_Z + (zc + 0)) * HellGen::NOISE_SIZE_Y + (yc + 0)];
				float s3 = noiseBuffer[((xc + 1) * HellGen::NOISE_SIZE_Z + (zc + 1)) * HellGen::NOISE_SIZE_Y + (yc + 0)];

				float s0a = (noiseBuffer[((xc + 0) * HellGen::NOISE_SIZE_Z + (zc + 0)) * HellGen::NOISE_SIZE_Y + (yc + 1)] - s0) * yStep;
				float s1a = (noiseBuffer[((xc + 0) * HellGen::NOISE_SIZE_Z + (zc + 1)) * HellGen::NOISE_SIZE_Y + (yc + 1)] - s1) * yStep;
				float s2a = (noiseBuffer[((xc + 1) * HellGen::NOISE_SIZE_Z + (zc + 0)) * HellGen::NOISE_SIZE_Y + (yc + 1)] - s2) * yStep;
				float s3a = (noiseBuffer[((xc + 1) * HellGen::NOISE_SIZE_Z + (zc + 1)) * HellGen::NOISE_SIZE_Y + (yc + 1)] - s3) * yStep;

				for (uint32_t y = 0; y < HellGen::CHUNK_HEIGHT; y++) {
					float xStep = 1 / (float)HellGen::CHUNK_WIDTH;

					float _s0 = s0;
					float _s1 = s1;
					float _s0a = (s2 - s0) * xStep;
					float _s1a = (s3 - s1) * xStep;

					for (uint32_t x = 0; x < HellGen::CHUNK_WIDTH; x++) {
						auto offs = (x + xc * HellGen::CHUNK_WIDTH) << (levelHeightBits + 4) | (0 + zc * HellGen::CHUNK_WIDTH) << levelHeightBits | (yc * HellGen::CHUNK_HEIGHT + y);
						float zStep = 1 / (float)HellGen::CHUNK_WIDTH;

						float val = _s0;
						float vala = (_s1 - _s0) * zStep;

						for (uint32_t z = 0; z < HellGen::CHUNK_WIDTH; z++) {
							BlockID blockId(BlockID::AIR);
							if (yc * HellGen::CHUNK_HEIGHT + y < (uint32_t)getDimension().getSeaLevel()) {
								blockId = Block::mStillLava->mID;
							}
							if (val > 0) {
								blockId = Block::mNetherrack->mID;
							}

							blocks.setBlock(offs, blockId);
							offs += step;
							val += vala;
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

void HellRandomLevelSource::buildSurfaces(BlockVolume& blocks, const ChunkPos& chunkPos) {

// 	auto& thread = generatorHelpersPool.getLocal();
// 	auto& random = thread.random;
// 	
// 	int waterHeight = blocks.getHeight() - 64;
// 
// 	Vec3 noisePos = Vec3(chunkPos) * 16.0f;
// 
// 	float sandBuffer[16 * 16];
// 	perlinNoise2->getRegion(sandBuffer, Vec3(noisePos.x, noisePos.z, 0.0f), 16, 16, 1, Vec3(1));
// 	float gravelBuffer[16 * 16];
// 	perlinNoise2->getRegion(gravelBuffer, Vec3(noisePos.x, 109.f, noisePos.z), 16, 1, 16, Vec3(1));
// 	float depthBuffer[16 * 16];
// 	perlinNoise3->getRegion(depthBuffer, Vec3(noisePos.x, noisePos.z, 0.0f), 16, 16, 1, Vec3(2));
// 
// 	for (int x = 0; x < 16; x++) {
// 		for (int z = 0; z < 16; z++) {
// 			bool sand = (sandBuffer[x + z * 16] + random.nextFloat() * 0.2f) > 0;
// 			bool gravel = (gravelBuffer[x + z * 16] + random.nextFloat() * 0.2f) > 0;
// 			int runDepth = (int)(depthBuffer[x + z * 16] / 3 + 3 + random.nextFloat() * 0.25f);
// 
// 			int run = -1;
// 
// 			BlockID top = Block::mNetherrack->mID;
// 			BlockID material = Block::mNetherrack->mID;
// 
// 			for (int y = (blocks.getHeight()-1); y >= 0; y--) {
// 				auto offs = blocks.index(Pos{ x, y, z });
// 
// 				uint32_t randUpperHeight = (blocks.getHeight() - 1) - random.nextInt(5);
// 				int randLowerHeight = random.nextInt(5);
// 				if ((uint32_t)y >= randUpperHeight || y <= randLowerHeight) {
// 					blocks.setBlock(offs, Block::mBedrock->mID);
// 				}
// 				else {
// 					const BlockID oldBlock(blocks.block(offs));
// 					if (oldBlock == 0) {
// 						run = -1;
// 					}
// 					else if (oldBlock == Block::mNetherrack->mID) {
// 						if (run == -1) {
// 							if (runDepth <= 0) {
// 								top = BlockID::AIR;
// 								material = Block::mNetherrack->mID;
// 							}
// 							else if (y >= waterHeight - 4 && y <= waterHeight + 1) {
// 								top = Block::mNetherrack->mID;
// 								material = Block::mNetherrack->mID;
// 								if (gravel) {
// 									top = Block::mGravel->mID;
// 									material = Block::mNetherrack->mID;
// 								}
// 
// 								if (sand) {
// 									top = Block::mSoulSand->mID;
// 									material = Block::mSoulSand->mID;
// 								}
// 							}
// 
// 							if (y < waterHeight && (top == BlockID::AIR || Block::mBlocks[top]->getMaterial().isType(MaterialType::Air))) {
// 								top = Block::mStillLava->mID;
// 							}
// 
// 							run = runDepth;
// 							if (y >= waterHeight - 1) {
// 								blocks.setBlock(offs, top);
// 							}
// 							else{
// 								blocks.setBlock(offs, material);
// 							}
// 						}
// 						else if (run > 0) {
// 							run--;
// 							blocks.setBlock(offs, material);
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
}

LevelChunk* HellRandomLevelSource::requestChunk(const ChunkPos& cp, LoadMode lm){
	return lm == LoadMode::None ? nullptr : new LevelChunk(*mLevel, *mDimension, cp);
}

void HellRandomLevelSource::loadChunk(LevelChunk& levelChunk, bool forceImmediateReplacementDataLoad) {

// 	auto& p = levelChunk.getPosition();
// 	auto& thread = generatorHelpersPool.getLocal();
// 	auto& random = thread.random;
// 
// 	random.setSeed((RandomSeed)(p.x * 341873128712l + p.z * 132897987541l));//@fix
// 
// 	memset(thread.blockBuffer.data(), 0, sizeof(thread.blockBuffer));
// 
// 	BlockVolume box(thread.blockBuffer, CHUNK_WIDTH, HellGen::GEN_DEPTH, CHUNK_WIDTH);
// 
// 	getDimension().getBiomes().fillBiomeData(&levelChunk, levelChunk.getMin().x, levelChunk.getMin().z);
// 
// 	prepareHeights(box, p.x, p.z);
// 	buildSurfaces(box, p);
// 	thread.hellCaveFeature.apply(box, levelChunk, getDimension(), random);
// 
// 	levelChunk.setAllBlockIDs(box.blocks, HellGen::GEN_DEPTH);
// 
// 	levelChunk.recalcHeightmap();
// 
// 	if (levelChunk.getGenerator() == nullptr) {
// 		levelChunk._setGenerator(this);
// 	}
// 
// 	levelChunk.changeState(ChunkState::Generating, ChunkState::Generated);
}

void HellRandomLevelSource::getHeights(float* noiseBuffer, int x, int y, int z) {

// 	ScopedProfile("getHeights");
// 
// 	float s = 1 * 684.412f;
// 	float hs = 1 * 684.412f * 3;
// 
// 	const int noiseSize2d = HellGen::NOISE_SIZE_X * HellGen::NOISE_SIZE_Y;
// 	Vec3 noisePos((float)x, (float)y, (float)z);
// 
// 	float scaleRegion[noiseSize2d];
// 	scaleNoise->getRegion(scaleRegion, noisePos, HellGen::NOISE_SIZE_X, 1, HellGen::NOISE_SIZE_Z, Vec3(1.0f, 0, 1.0f));
// 	float depthRegion[noiseSize2d];
// 	depthNoise->getRegion(depthRegion, noisePos, HellGen::NOISE_SIZE_X, 1, HellGen::NOISE_SIZE_Z, Vec3(100.0f, 0, 100.0f));
// 
// 	const int noiseSize3d = HellGen::NOISE_SIZE_X * HellGen::NOISE_SIZE_Y * HellGen::NOISE_SIZE_Z;
// 
// 	float noiseRegionPrimary[noiseSize3d];
// 	perlinNoise1->getRegion(noiseRegionPrimary, noisePos, HellGen::NOISE_SIZE_X, HellGen::NOISE_SIZE_Y, HellGen::NOISE_SIZE_Z, Vec3(s / 80.0f, hs / 60.0f, s / 80.0f));
// 	float noiseRegionA[noiseSize3d];
// 	lperlinNoise1->getRegion(noiseRegionA, noisePos, HellGen::NOISE_SIZE_X, HellGen::NOISE_SIZE_Y, HellGen::NOISE_SIZE_Z, Vec3(s, hs, s));
// 	float noiseRegionB[noiseSize3d];
// 	lperlinNoise2->getRegion(noiseRegionB, noisePos, HellGen::NOISE_SIZE_X, HellGen::NOISE_SIZE_Y, HellGen::NOISE_SIZE_Z, Vec3(s, hs, s));
// 
// 	int p = 0;
// 	float yoffs[HellGen::NOISE_SIZE_Y];
// 
// 	for (int yy = 0; yy < HellGen::NOISE_SIZE_Y; yy++) {
// 		yoffs[yy] = Math::cos(yy * PI * 6 / (float)HellGen::NOISE_SIZE_Y) * 2;
// 
// 		float dd = (float)yy;
// 		if (yy > HellGen::NOISE_SIZE_Y / 2) {
// 			dd = (float)(HellGen::NOISE_SIZE_Y - 1) - yy;
// 		}
// 		if (dd < 4) {
// 			dd = 4 - dd;
// 			yoffs[yy] -= dd * dd * dd * 10;
// 		}
// 	}
// 
// 	for (int xx = 0; xx < HellGen::NOISE_SIZE_X; xx++) {
// 
// 		for (int zz = 0; zz < HellGen::NOISE_SIZE_Z; zz++) {
// 
// 			float floating = 0;
// 
// 			for (int yy = 0; yy < HellGen::NOISE_SIZE_Y; yy++) {
// 				float val = 0;
// 
// 				float yOffs = yoffs[yy];
// 
// 				float bb = noiseRegionA[p] / 512;
// 				float cc = noiseRegionB[p] / 512;
// 
// 				float v = (noiseRegionPrimary[p] / 10 + 1) / 2;
// 				if (v < 0) {
// 					val = bb;
// 				}
// 				else if (v > 1) {
// 					val = cc;
// 				}
// 				else{
// 					val = bb + (cc - bb) * v;
// 				}
// 				val -= yOffs;
// 
// 				if (yy > HellGen::NOISE_SIZE_Y - 4) {
// 					float slide = (yy - (HellGen::NOISE_SIZE_Y - 4)) / (4 - 1.0f);
// 					val = val * (1 - slide) + -10 * slide;
// 				}
// 
// 				if (yy < floating) {
// 					float slide = (floating - yy) / (4);
// 					if (slide < 0) {
// 						slide = 0;
// 					}
// 					if (slide > 1) {
// 						slide = 1;
// 					}
// 					val = val * (1 - slide) + -10 * slide;
// 				}
// 
// 				noiseBuffer[p] = val;
// 				p++;
// 			}
// 		}
// 	}
}

bool HellRandomLevelSource::postProcess(ChunkViewSource& neighborhood) {

// 	//grab the center chunk
// 	auto& nmin = neighborhood.getArea().getBounds().mMin;
// 	ChunkPos centerPos(nmin.x + 1, nmin.z + 1);
// 
// 	auto lc = neighborhood.getExistingChunk(centerPos);
// 	auto &lcPosition = lc->getPosition();
// 
// 	DEBUG_ASSERT(centerPos == lcPosition, "the chunk is in the wrong position, something's up mang");
// 	DEBUG_ASSERT(neighborhood.getArea().getBounds().mVolume == 9, "Wrong number of chunks in the PP neighborhood");
// 
// 	//the ownership is automatically released (RAII FTW)
// 	auto ownsNeighborhood = PostprocessingManager::instance.tryLock(lcPosition);
// 	if (!ownsNeighborhood) {
// 		return false;
// 	}
// 
// 	bool forceSave = false;
// 
// 	{
// 		ScopedProfile("postProcessStart");
// 		//create a source allowed to read non-populated chunks
// 		BlockSource source(neighborhood.getLevel(), neighborhood.getDimension(), neighborhood, false, true);
// 
// 		//create a place where to store all the ticks
// 		BlockTickingQueue instaTickQueue;
// 		source.setTickingQueue(instaTickQueue);
// 
// 		auto& thread = generatorHelpersPool.getLocal();
// 		auto& random = thread.random;
// 
// 		const int xo = lcPosition.x * 16;
// 		const int zo = lcPosition.z * 16;
// 		BlockPos origin(xo, 0, zo);
// 
// 		netherBridgeFeature.apply(*lc, getDimension(), random);
// 
// 		// restore seed after calls to "apply" and before any calls to "postProcess"
// 		random.setSeed(mLevel->getSeed());
// 		int xScale = random.nextInt() / 2 * 2 + 1;
// 		int zScale = random.nextInt() / 2 * 2 + 1;
// 		random.setSeed(((lcPosition.x * xScale) + (lcPosition.z * zScale)) ^ mLevel->getSeed());
// 
// 		forceSave |= netherBridgeFeature.postProcess(&source, random, lcPosition.x, lcPosition.z);
// 
// 		HellSpringFeature hellSpringFeature(Block::mFlowingLava->mID, false);
// 
// 		for (int i = 0; i < 8; i++) {
// 			int x = random.nextInt(16) + 8;
// 			int y = random.nextInt(HellGen::GEN_DEPTH - 8) + 4;
// 			int z = random.nextInt(16) + 8;
// 			BlockPos featurePlacement(origin.offset(x, y, z));
// 			hellSpringFeature.place(source, featurePlacement, random);
// 			instaTickQueue.add(source, featurePlacement, Block::mFlowingLava->mID, 0);
// 		}
// 
// 		HellFireFeature hellFireFeature;
// 		int randc = random.nextInt(10) + 1;
// 		int count = random.nextInt(randc) + 1;
// 
// 		for (int i = 0; i < count; i++) {
// 			int x = random.nextInt(16) + 8;
// 			int y = random.nextInt(HellGen::GEN_DEPTH - 8) + 4;
// 			int z = random.nextInt(16) + 8;
// 			BlockPos featurePlacement(origin.offset(x, y, z));
// 			hellFireFeature.place(source, featurePlacement, random);
// 		}
// 
// 		GlowStoneFeature glowStoneFeature;
// 		randc = random.nextInt(10) + 1;
// 		count = random.nextInt(randc);
// 		for (int i = 0; i < count; i++) {
// 			int x = random.nextInt(16) + 8;
// 			int y = random.nextInt(HellGen::GEN_DEPTH - 8) + 4;
// 			int z = random.nextInt(16) + 8;
// 			BlockPos featurePlacement(origin.offset(x, y, z));
// 			glowStoneFeature.place(source, featurePlacement, random);
// 		}
// 		for (int i = 0; i < 10; i++) {
// 			int x = random.nextInt(16) + 8;
// 			int y = random.nextInt(HellGen::GEN_DEPTH);
// 			int z = random.nextInt(16) + 8;
// 			BlockPos featurePlacement(origin.offset(x, y, z));
// 			glowStoneFeature.place(source, featurePlacement, random);
// 		}
// 
// 		FlowerFeature brownMushroomFeature(Block::mBrownMushroom->mID);
// 		if (random.nextInt(1) == 0) {
// 			int x = random.nextInt(16) + 8;
// 			int y = random.nextInt(HellGen::GEN_DEPTH);
// 			int z = random.nextInt(16) + 8;
// 			BlockPos featurePlacement(origin.offset(x, y, z));
// 			brownMushroomFeature.place(source, featurePlacement, random);
// 		}
// 
// 		FlowerFeature redMushroomFeature(Block::mRedMushroom->mID);
// 		if (random.nextInt(1) == 0) {
// 			int x = random.nextInt(16) + 8;
// 			int y = random.nextInt(HellGen::GEN_DEPTH);
// 			int z = random.nextInt(16) + 8;
// 			BlockPos featurePlacement(origin.offset(x, y, z));
// 			redMushroomFeature.place(source, featurePlacement, random);
// 		}
// 
// 		OreFeature quartzFeature(Block::mQuartzOre->mID, 14);
// 
// 		for (int i = 0; i < 16; i++) {
// 			int x = random.nextInt(16);
// 			int y = random.nextInt(HellGen::GEN_DEPTH - 20) + 10;
// 			int z = random.nextInt(16);
// 			BlockPos featurePlacement(origin.offset(x, y, z));
// 			quartzFeature.place(source, featurePlacement, random);
// 		}
// 
// 		HellSpringFeature lavaTrapFeature(Block::mFlowingLava->mID, true);
// 
// 		for (int i = 0; i < 16; i++) {
// 			int x = random.nextInt(16);
// 			int y = random.nextInt(HellGen::GEN_DEPTH - 20) + 10;
// 			int z = random.nextInt(16);
// 			BlockPos featurePlacement(origin.offset(x, y, z));
// 			lavaTrapFeature.place(source, featurePlacement, random);
// 			instaTickQueue.add(source, featurePlacement, Block::mFlowingLava->mID, 0);
// 		}
// 
// 		instaTickQueue.tickAllPendingTicks(source);
// 
// 		lc->updateLightsAndHeights(source);
// 
// 		if (forceSave == true) {
// 			lc->setUnsaved();
// 		}
// 	}

	return true;
}

const MobList& HellRandomLevelSource::getMobsAt(BlockSource& blockSource, EntityType category, const BlockPos& pos) {

	if (EntityClassTree::isTypeInstanceOf(category, EntityType::Monster)) {
		if (netherBridgeFeature.isInsideFeature(pos.x, pos.y, pos.z)) {
			return netherBridgeFeature.getBridgeEnemies();
		}
		if ((netherBridgeFeature.isInsideBoundingFeature(pos.x, pos.y, pos.z) && blockSource.getBlockID(pos.below()) == Block::mNetherBrick->mID)) {
			return netherBridgeFeature.getBridgeEnemies();
		}
	}

	return Biome::EMPTY_MOBLIST;
}

std::string HellRandomLevelSource::gatherStats() {
	return "HellRandomLevelSource";
}

bool HellRandomLevelSource::findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) {
	if (feature == StructureFeatureType::Fortress) {
		return netherBridgeFeature.getNearestGeneratedFeature(player, pos);
	}

	return false;
}

Unique<CompoundTag> HellRandomLevelSource::getStructureTag() const {
	Unique<CompoundTag> structures = make_unique<CompoundTag>();
	structures->putCompound("bridge", netherBridgeFeature.getStructureTag());
	return structures;
}

void HellRandomLevelSource::readStructureTag(const CompoundTag& tag) {
	const CompoundTag* compoundTag = tag.getCompound("bridge");
	if (compoundTag) {
		netherBridgeFeature.readStructureTag(*compoundTag, getLevel());
	}
}

