/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/v1/TheEndRandomLevelSource.h"

// #include "util/PerfTimer.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/level/biome/Biome.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/biome/TheEndBiomeDecorator.h"
#include "world/level/block/Block.h"
#include "world/level/block/ChorusFlowerBlock.h"
// #include "world/level/block/entity/EndGatewayBlockEntity.h"
#include "world/level/chunk/ChunkViewSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/chunk/PostprocessingManager.h"
#include "world/level/dimension/Dimension.h"
// #include "world/level/dimension/end/TheEndDimension.h"
#include "world/level/Level.h"
#include "world/level/levelgen/feature/EndIslandFeature.h"
#include "world/level/levelgen/feature/EndGatewayFeature.h"
#include "world/level/levelgen/structure/StructureFeatureType.h"
#include "world/level/levelgen/synth/PerlinNoise.h"
#include "world/level/levelgen/synth/SimplexNoise.h"
#include "world/level/levelgen/v1/EndGatewayChunkSource.h"
#include "world/level/material/Material.h"
#include "world/phys/Vec2.h"
#include "world/level/block/BlockVolume.h"

TheEndRandomLevelSource::TheEndRandomLevelSource(Level* level, Dimension* dimension, RandomSeed seed)
	: ChunkSource(level, dimension, CHUNK_WIDTH)
	, mEndCityFeature(*this)
// 	, generatorHelpersPool([&]() {
// 	return make_unique<ThreadData>();
// }) 
{

	Random random(seed);

	mLPerlinNoise1 = make_unique<PerlinNoise>(random, 16);
	mLPerlinNoise2 = make_unique<PerlinNoise>(random, 16);
	mPerlinNoise1 = make_unique<PerlinNoise>(random, 8);

	mIslandNoise = make_unique<SimplexNoise>(random);
}

LevelChunk* TheEndRandomLevelSource::requestChunk(const ChunkPos& cp, LoadMode lm) {
	return lm == LoadMode::None ? nullptr : new LevelChunk(*mLevel, *mDimension, cp);
}

void TheEndRandomLevelSource::loadChunk(LevelChunk& levelChunk, bool forceImmediateReplacementDataLoad) {
// 	auto& p = levelChunk.getPosition();
// 	auto& thread = generatorHelpersPool.getLocal();
// 
// 	getDimension().getBiomes().fillBiomeData(&levelChunk, levelChunk.getMin().x, levelChunk.getMin().z);
// 
// 	memset(thread.blockBuffer.data(), 0, sizeof(thread.blockBuffer));
// 
// 	BlockVolume box(thread.blockBuffer, CHUNK_WIDTH, TheEndGen::GEN_DEPTH, CHUNK_WIDTH);
// 
// 	prepareHeights(box, p.x, p.z);
// 	buildSurfaces(box, p);
// 
// 	levelChunk.setAllBlockIDs(box.blocks, box.getHeight());
// 	auto subChunks = levelChunk.getSubChunks();
// 	for (auto& subChunk : subChunks) {
// 		void* skyData= subChunk->mSkyLight.data();
// 		memset(skyData, 0, subChunk->mSkyLight.BYTES);
// 	}
// 
// 	levelChunk.recalcHeightmap();
// 
// 	if (levelChunk.getGenerator() == nullptr) {
// 		levelChunk._setGenerator(this);
// 	}
// 
// 	levelChunk.changeState(ChunkState::Generating, ChunkState::Generated);

}

void TheEndRandomLevelSource::prepareHeights(BlockVolume& box, int xOffs, int zOffs) {
// 	ScopedProfile("prepareHeights");

	int levelHeightBits = (int)Math::log2((float)box.getHeight());
	int step = 1 << levelHeightBits;

	float noiseBuffer[TheEndGen::NOISE_SIZE_X * TheEndGen::NOISE_SIZE_Y * TheEndGen::NOISE_SIZE_Z];
	getHeights(noiseBuffer, xOffs * TheEndGen::CHUNKCOUNT_XZ, 0, zOffs * TheEndGen::CHUNKCOUNT_XZ);

	for (int xc = 0; xc < TheEndGen::CHUNKCOUNT_XZ; ++xc) {
		for (int zc = 0; zc < TheEndGen::CHUNKCOUNT_XZ; ++zc) {
			for (int yc = 0; yc < TheEndGen::GEN_DEPTH / TheEndGen::CHUNK_HEIGHT; ++yc) {
				const float yStep = 1 / (float)TheEndGen::CHUNK_HEIGHT;
				float s0 = noiseBuffer[((xc + 0) * TheEndGen::NOISE_SIZE_Z + zc + 0) * TheEndGen::NOISE_SIZE_Y + yc + 0];
				float s1 = noiseBuffer[((xc + 0) * TheEndGen::NOISE_SIZE_Z + zc + 1) * TheEndGen::NOISE_SIZE_Y + yc + 0];
				float s2 = noiseBuffer[((xc + 1) * TheEndGen::NOISE_SIZE_Z + zc + 0) * TheEndGen::NOISE_SIZE_Y + yc + 0];
				float s3 = noiseBuffer[((xc + 1) * TheEndGen::NOISE_SIZE_Z + zc + 1) * TheEndGen::NOISE_SIZE_Y + yc + 0];

				const float s0a = (noiseBuffer[((xc + 0) * TheEndGen::NOISE_SIZE_Z + zc + 0) * TheEndGen::NOISE_SIZE_Y + yc + 1] - s0) * yStep;
				const float s1a = (noiseBuffer[((xc + 0) * TheEndGen::NOISE_SIZE_Z + zc + 1) * TheEndGen::NOISE_SIZE_Y + yc + 1] - s1) * yStep;
				const float s2a = (noiseBuffer[((xc + 1) * TheEndGen::NOISE_SIZE_Z + zc + 0) * TheEndGen::NOISE_SIZE_Y + yc + 1] - s2) * yStep;
				const float s3a = (noiseBuffer[((xc + 1) * TheEndGen::NOISE_SIZE_Z + zc + 1) * TheEndGen::NOISE_SIZE_Y + yc + 1] - s3) * yStep;

				for (int y = 0; y < TheEndGen::CHUNK_HEIGHT; ++y) {
					const float xStep = 1 / (float)TheEndGen::CHUNK_WIDTH;

					float _s0 = s0;
					float _s1 = s1;
					const float _s0a = (s2 - s0) * xStep;
					const float _s1a = (s3 - s1) * xStep;

					for (int x = 0; x < TheEndGen::CHUNK_WIDTH; ++x) {
						int offs = (x + xc * TheEndGen::CHUNK_WIDTH) << (levelHeightBits + 4) |
								   (0 + zc * TheEndGen::CHUNK_WIDTH) << levelHeightBits |
								   (y + yc * TheEndGen::CHUNK_HEIGHT);

						const float zStep = 1 / (float)TheEndGen::CHUNK_WIDTH;

						float val = _s0;
						const float vala = (_s1 - _s0) * zStep;

						for (int z = 0; z < TheEndGen::CHUNK_WIDTH; ++z) {
							BlockID blockId(BlockID::AIR);
							if (val > 0.0f) {
								blockId = Block::mEndStone->mID;
							}

							box.blocks[offs] = blockId;
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

void TheEndRandomLevelSource::buildSurfaces(BlockVolume& box, const ChunkPos& chunkPos) {
	for (int x = 0; x < 16; ++x) {
		for (int z = 0; z < 16; ++z) {
			int run = -1;

			BlockID top = Block::mEndStone->mID;
			BlockID material = Block::mEndStone->mID;

			for (int y = (box.getHeight() - 1); y >= 0; y--) {
				int offs = (z * 16 + x) * box.getHeight() + y;
				const BlockID block(box.blocks[offs]);

				if (Block::mBlocks[block]->getMaterial().isType(MaterialType::Air)) {
					run = -1;
				}
				else if (block == Block::mStone->mID) {
					if (run == -1) {
						run = 1;
						if (y >= 0) {
							box.blocks[offs] = top;
						}
						else {
							box.blocks[offs] = material;
						}
					}
					else if (run > 0) {
						--run;
						box.blocks[offs] = material;
					}
				}
			}
		}
	}
}

void TheEndRandomLevelSource::getHeights(float* noiseBuffer, int x, int y, int z) {
// 	ScopedProfile("getHeights");

	float s = 1 * 684.412f * 2.0f;
	float hs = 1 * 684.412f;

	const int noiseSize3d = TheEndGen::NOISE_SIZE_X * TheEndGen::NOISE_SIZE_Y * TheEndGen::NOISE_SIZE_Z;
	BlockPos noisePos(x, y, z);

	float noiseRegionPrimary[noiseSize3d];
	mPerlinNoise1->getRegion(noiseRegionPrimary, noisePos, TheEndGen::NOISE_SIZE_X, TheEndGen::NOISE_SIZE_Y, TheEndGen::NOISE_SIZE_Z, Vec3(s / 80.0f, hs / 160.0f, s / 80.0f));
	float noiseRegionA[noiseSize3d];
	mLPerlinNoise1->getRegion(noiseRegionA, noisePos, TheEndGen::NOISE_SIZE_X, TheEndGen::NOISE_SIZE_Y, TheEndGen::NOISE_SIZE_Z, Vec3(s, hs, s));
	float noiseRegionB[noiseSize3d];
	mLPerlinNoise2->getRegion(noiseRegionB, noisePos, TheEndGen::NOISE_SIZE_X, TheEndGen::NOISE_SIZE_Y, TheEndGen::NOISE_SIZE_Z, Vec3(s, hs, s));

	const int chunkX = x / 2;
	const int chunkZ = z / 2;

	int p = 0;

	for (int xx = 0; xx < TheEndGen::NOISE_SIZE_X; ++xx) {
		for (int zz = 0; zz < TheEndGen::NOISE_SIZE_Z; ++zz) {
			const float doffs = getIslandHeightValue(chunkX, chunkZ, xx, zz);

			for (int yy = 0; yy < TheEndGen::NOISE_SIZE_Y; ++yy) {
				float bb = noiseRegionA[p] / 512.0f;
				float cc = noiseRegionB[p] / 512.0f;

				float val = 0.0f;
				float v = (noiseRegionPrimary[p] / 10.0f + 1.0f) / 2.0f;
				if (v < 0.0f) {
					val = bb;
				}
				else if (v > 1.0f) {
					val = cc;
				}
				else {
					val = bb + (cc - bb) * v;
				}
				val -= 8.0f;

				val += doffs;

				int r = 2;
				if (yy > TheEndGen::NOISE_SIZE_Y / 2 - r) {
					float slide = (yy - (TheEndGen::NOISE_SIZE_Y / 2 - r)) / 64.0f;
					slide = Math::clamp(slide, 0.0f, 1.0f);
					val = val * (1.0f - slide) + -3000.0f * slide;
				}

				r = 8;
				if (yy < r) {
					float slide = (r - yy) / (r - 1.0f);
					val = val * (1.0f - slide) + -30.0f * slide;
				}

				noiseBuffer[p] = val;
				p++;
			}
		}
	}
}

float TheEndRandomLevelSource::getIslandHeightValue(int chunkX, int chunkZ, int subSectionX, int subSectionZ) {
	// Calculate default value.
	float xd = static_cast<float>(chunkX * 2 + subSectionX);
	float zd = static_cast<float>(chunkZ * 2 + subSectionZ);
	float doffs = 100 - Math::sqrt(xd * xd + zd * zd) * 8;
	doffs = Math::clamp(doffs, -100.0f, 80.0f);

	// Find other islands from -12 chunks to +12 chunks.
	for (int xo = -12; xo <= 12; ++xo) {
		for (int zo = -12; zo <= 12; ++zo) {
			const int64_t totalChunkX = chunkX + xo;
			const int64_t totalChunkZ = chunkZ + zo;

			if (totalChunkX * totalChunkX + totalChunkZ * totalChunkZ > TheEndGen::ISLAND_CHUNK_DISTANCE_SQR &&
				mIslandNoise->_getValue(Vec2((float)totalChunkX, (float)totalChunkZ)) < TheEndGen::ISLAND_THRESHOLD) {

				const float islandSize = static_cast<float>(((std::abs(totalChunkX) * 3439 + std::abs(totalChunkZ) * 147) % 13) + 9);
				xd = static_cast<float>(subSectionX - xo * 2);
				zd = static_cast<float>(subSectionZ - zo * 2);

				float newDoffs = 100.0f - Math::sqrt(xd * xd + zd * zd) * islandSize;
				newDoffs = Math::clamp(newDoffs, -100.0f, 80.0f);

				if (newDoffs > doffs) {
					doffs = newDoffs;
				}
			}
		}
	}

	return doffs;
}

bool TheEndRandomLevelSource::postProcess(ChunkViewSource& neighborhood) {
	// Grab the center chunk
	auto& nmin = neighborhood.getArea().getBounds().mMin;
	ChunkPos centerPos(nmin.x + 1, nmin.z + 1);

	auto levelChunk = neighborhood.getExistingChunk(centerPos);
	auto &lcPosition = levelChunk->getPosition();

	DEBUG_ASSERT(centerPos == lcPosition, "the chunk is in the wrong position");
	DEBUG_ASSERT(neighborhood.getArea().getBounds().mVolume == 9, "Wrong number of chunks");

	auto ownsNeighborhood = PostprocessingManager::instance.tryLock(lcPosition);
	if (!ownsNeighborhood) {
		return false;
	}

	bool forceSave = false;

	{
// 		ScopedProfile("postProcessStart");

		// Create a source allowed to read non-populated chunks
		BlockSource source(neighborhood.getLevel(), neighborhood.getDimension(), neighborhood, false, true);

		// Create a place where to store all the ticks
// 		BlockTickingQueue instaTickQueue;
// 		source.setTickingQueue(instaTickQueue);

		BlockPos origin(lcPosition);
		Random random(RandomSeed(lcPosition.x * 341873128712l + lcPosition.z * 132897987541l));

		{
// 			ScopedProfile("endCityFeature");
			mEndCityFeature.apply(*levelChunk, getDimension(), random);
		}

		// Restore seed after calls to "apply" and before any calls to "postProcess"
		random.setSeed(mLevel->getSeed());
		int xScale = random.nextInt() / 2 * 2 + 1;
		int zScale = random.nextInt() / 2 * 2 + 1;
		random.setSeed(((lcPosition.x * xScale) + (lcPosition.z * zScale)) ^ mLevel->getSeed());

		{
// 			ScopedProfile("endCityPostProcess");
			forceSave |= mEndCityFeature.postProcess(&source, random, lcPosition.x, lcPosition.z);
		}

		{
// 			ScopedProfile("biomeDecorate");
			source.getBiome(levelChunk->getMax()).decorate(&source, random, levelChunk->getMin(), false, FLT_MAX);
		}

		int64_t distance = lcPosition.x * lcPosition.x + lcPosition.z * lcPosition.z;
		if (distance > TheEndGen::ISLAND_CHUNK_DISTANCE_SQR) {
			const float islandHeightValue = getIslandHeightValue(lcPosition.x, lcPosition.z, 1, 1);

			if (islandHeightValue < -20.0f && random.nextInt(14) == 0) {
// 				ScopedProfile("endIslandFeature");
				int x = random.nextInt(16) + 8;
				int y = random.nextInt(16) + 55;
				int z = random.nextInt(16) + 8;
				BlockPos featurePlacement(origin.offset(x, y, z));

				EndIslandFeature endIslands;
				endIslands.place(source, featurePlacement, random);

				if (random.nextInt(4) == 0) {
					endIslands.place(source, featurePlacement, random);
				}
			}

			if (getIslandHeightValue(lcPosition.x, lcPosition.z, 1, 1) > 40.0f) {
// 				ScopedProfile("tryPlaceChorusPlants");
				const int chorusPlantCount = random.nextInt(5);
				for (int i = 0; i < chorusPlantCount; ++i) {
					int x = random.nextInt(16) + 8;
					int z = random.nextInt(16) + 8;
					Height heightmap = source.getHeightmap(origin.offset(x, 0, z));
					if (heightmap > 0) {
						int y = heightmap - 1;

						if (source.isEmptyBlock(origin.offset(x, y + 1, z)) && source.getBlockID(origin.offset(x, y, z)) == Block::mEndStone->mID) {
							ChorusFlowerBlock::generatePlant(source, origin.offset(x, y + 1, z), random, 8);
						}
					}
				}

				if (random.nextInt(700) == 0) {
					int x = random.nextInt(16) + 8;
					int z = random.nextInt(16) + 8;
					Height heightmap = source.getHeightmap(origin.offset(x, 0, z));
					if (heightmap > 0) {
						int y = heightmap + 3 + random.nextInt(7);
						BlockPos gatewayPos = origin.offset(x, y, z);
						EndGatewayFeature().place(source, gatewayPos, random);
						BlockEntity* exitEntity = source.getBlockEntity(gatewayPos);
// 						if (exitEntity && exitEntity->getType() == BlockEntityType::EndGateway) {
// 							EndGatewayBlockEntity* exitGateway = static_cast<EndGatewayBlockEntity*>(exitEntity);
// 							exitGateway->setExitPosition(mDimension->getSpawnPos(), true);
// 						}
					}
				}

			}
		}

// 		{
// 			TheEndDimension* theEndDimension = static_cast<TheEndDimension*>(mDimension);
// 			auto& minPos = levelChunk->getMin();
// 			auto& maxPos = levelChunk->getMax();
// 
// 			auto gatewayChunkSource = theEndDimension->getGatewayChunk(BoundingBox(minPos.x, 0, minPos.z, maxPos.x, TheEndGen::GEN_DEPTH, maxPos.z));
// 			if (gatewayChunkSource != nullptr) {
// 				ScopedProfile("endGatewayExit");
// 
// 				// Attempt to find a valid spawn point
// 				BlockPos exitPortal = EndGatewayBlockEntity::findValidSpawnInChunk(*levelChunk);
// 
// 				if (exitPortal == BlockPos::ZERO) {
// 					exitPortal = BlockPos(gatewayChunkSource->getExitOrigin().x, 75, gatewayChunkSource->getExitOrigin().z);
// 
// 					// Failed to find a suitable block to spawn, create an island
// 					EndIslandFeature endIslandFeature;
// 					Random rand(RandomSeed(exitPortal.hashCode()));
// 					endIslandFeature.place(source, exitPortal, rand);
// 				}
// 
// 				// Find exit position
// 				exitPortal = EndGatewayBlockEntity::findTallestBlock(source, exitPortal, CHUNK_WIDTH, true);
// 				exitPortal = exitPortal.above(10);
// 
// 				// Find entry chunk source
// 				auto& entryPortal = gatewayChunkSource->getEntryPos();
// 				auto entryChunkSource = theEndDimension->getGatewayChunkAt(entryPortal);
// 				DEBUG_ASSERT(entryChunkSource != nullptr, "The entry chunk source should have been created when the exit -gatewayChunkSource- was.");
// 
// 				auto entryChunk = entryChunkSource->getExistingChunk(entryPortal);
// 				DEBUG_ASSERT(entryChunk != nullptr, "The entry chunk should have been saved in the EndGatewayChunkSource.");
// 
// 				// Create exit portal and update both gateways
// 				EndGatewayBlockEntity::createExitPortal(source, exitPortal, *entryChunk, entryPortal);
// 			}
// 		}
// 
// 		{
// 			ScopedProfile("instaTicks");
// 			instaTickQueue.tickAllPendingTicks(source);
// 		}
// 
// 		{
// 			ScopedProfile("postProcessLightsAndHeights");
// 			levelChunk->updateLightsAndHeights(source);
// 		}

		if (forceSave) {
			levelChunk->setUnsaved();
		}
	}

	return true;
}

void TheEndRandomLevelSource::postProcessMobsAt(BlockSource* blockSource, int chunkWestBlock, int chunkNorthBlock, Random& random) {
	auto spikes = TheEndBiomeDecorator::getSpikesForLevel(blockSource->getLevel());
	for (auto&& spike : spikes) {
		if (spike.startsInChunk(BlockPos(chunkWestBlock, 0, chunkNorthBlock))) {
			SpikeFeature spikeFeature(spike);
			spikeFeature.postProcessMobsAt(*blockSource, BlockPos(spike.getCenterX(), 45, spike.getCenterZ()), random);
		}
	}

	mEndCityFeature.postProcessMobsAt(blockSource, chunkWestBlock, chunkNorthBlock, random);
}

const MobList& TheEndRandomLevelSource::getMobsAt(BlockSource& blockSource, EntityType category, const BlockPos& pos) {
	return blockSource.getBiome(pos).getMobs(category);
}

bool TheEndRandomLevelSource::isIslandChunk(int chunkX, int chunkZ) {
	return ((int64_t)chunkX * (int64_t)chunkX + (int64_t)chunkZ * (int64_t)chunkZ) > TheEndGen::ISLAND_CHUNK_DISTANCE_SQR &&
		getIslandHeightValue(chunkX, chunkZ, 1, 1) >= 0;
}

std::string TheEndRandomLevelSource::gatherStats() {
	return "TheEndRandomLevelSource";
}

bool TheEndRandomLevelSource::findNearestFeature(StructureFeatureType feature, Player& player, BlockPos& pos) {
	if (feature == StructureFeatureType::EndCity) {
		return mEndCityFeature.getNearestGeneratedFeature(player, pos);
	}

	return false;
}

Unique<CompoundTag> TheEndRandomLevelSource::getStructureTag() const {
	Unique<CompoundTag> structures = make_unique<CompoundTag>();
	structures->putCompound("endcity", mEndCityFeature.getStructureTag());
	return structures;
}

void TheEndRandomLevelSource::readStructureTag(const CompoundTag& tag) {
	const CompoundTag* compoundTag = tag.getCompound("endcity");
	if (compoundTag) {
		mEndCityFeature.readStructureTag(*compoundTag, getLevel());
	}
}
