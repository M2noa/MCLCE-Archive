#include "Dungeons.h"
#include "world/level/biome/MesaBiome.h"
#include "world/level/block/Block.h"
#include "world/level/block/SandBlock.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/block/DirtBlock.h"
//#include "world/item/DyePowderItem.h"
#include "world/level/material/Material.h"
#include "world/level/block/BlockVolume.h"

MesaBiome::MesaBiome(int id, bool brycePillars, bool hasForest) :
	Biome(id, Biome::BiomeType::Mesa, make_unique<Decorator>())
	, brycePillars(brycePillars)
	, hasForest(hasForest) {
	setNoRain();
	setTemperatureAndDownfall(2, 0);

	_friendlies.clear();
	mTopMaterial = Block::mSand->mID;
	mTopMaterialData = enum_cast(SandBlock::SandType::Red);
	mMaterial = Block::mHardenedClay->mID;

	decorator->treeCount = -999;
	decorator->deadBushCount = 20;
	decorator->reedsCount = 3;
	decorator->cactusCount = 5;
	decorator->flowerCount = 0;

	if(hasForest) {
		decorator->treeCount = 5;
	}
}

const FeaturePtr& MesaBiome::getTreeFeature(Random* random){
	return decorator->oakFeature;
}

int MesaBiome::getFoliageColor() {
	return 0xffaea42a;
}

int MesaBiome::getMapFoliageColor() {
	return getFoliageColor();
}

int MesaBiome::getGrassColor( const BlockPos& pos ) {
	return 0xff90814d;
}

int MesaBiome::getMapGrassColor(const BlockPos& pos) {
	return getGrassColor(pos);
}

void MesaBiome::refreshBiome(RandomSeed levelSeed) {
	generateBands(levelSeed);
	Random r(levelSeed);
	pillarNoise = make_unique<PerlinSimplexNoise>(r, 4);
	pillarRoofNoise = make_unique<PerlinSimplexNoise>(r, 1);
}

void MesaBiome::buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) {

	//_placeBedrock(random, blocks, pos);

	//float minPillar = 0;
	//if(brycePillars) {
	//	const int noiseX = pos.x;
	//	const int noiseZ = pos.z;

	//	float pillarBuffer = Math::min(std::abs(depthValue), pillarNoise->getValue(noiseX * 0.25f, noiseZ * 0.25f));
	//	if (pillarBuffer > 0) {
	//		float floorScale = 1.0f / 512.0f;
	//		float pillarFloor = std::abs(pillarRoofNoise->getValue(noiseX * floorScale, noiseZ * floorScale));
	//		minPillar = pillarBuffer * pillarBuffer * 2.5f;
	//		float max = static_cast<float>(Math::ceil(pillarFloor * 50.0f) + 14.0f);
	//		if (minPillar > max) {
	//			minPillar = max;
	//		}
	//		minPillar += 64.0f;
	//	}
	//}
	//auto p = (ChunkBlockPos)pos;

	//auto clay = Block::mStainedClay->mID;
	//auto hardClay = Block::mHardenedClay->mID;
	//BlockID top = clay;
	//BlockID material = mMaterial;

	//int runDepth = (int)(depthValue / 3 + 3 + random.nextFloat() * 0.25f);
	//bool claySurface = std::cos(depthValue/3.0f * PI) > 0.0f;
	//int run = -1;
	//bool sandRun = false;
	//int clayDepth = 0;

	//for(p.y = (LEVEL_GEN_HEIGHT - 1); p.y > 0; p.y--) {

	//	auto old = blocks.block(p);

	//	if (old == Block::mBedrock->mID) {
	//		break;
	//	}

	//	auto& oldMaterial = Block::mBlocks[old]->getMaterial();
	//	if((oldMaterial.isType(MaterialType::Air) || oldMaterial.isType(MaterialType::Water)) && p.y < (int)minPillar) {
	//		blocks.setBlock(p, Block::mStone->mID);
	//		old = Block::mStone->mID;
	//	}

	//	if (clayDepth < 15) {
	//		if(old == 0) {
	//			run = -1;
	//		}
	//		else if (old == Block::mStone->mID) {
	//			if (run == -1) {
	//				sandRun = false;
	//				if (runDepth <= 0) {
	//					top = BlockID::AIR;
	//					material = Block::mStone->mID;
	//				}
	//				else if (p.y >= seaLevel - 4 && p.y <= seaLevel + 1) {
	//					top = clay;
	//					material = mMaterial;
	//				}

	//				if (p.y < seaLevel && top == 0) {
	//					top = Block::mStillWater->mID;
	//				}

	//				run = runDepth + std::max(0, p.y - seaLevel);

	//				if (p.y >= seaLevel - 1) {
	//					if (hasForest && p.y > 86 + runDepth * 2) {
	//						if (claySurface) {
	//							blocks.setBlock(p, Block::mDirt->mID); //TODO: Make this coarse dirt

	//							DataID data = 0;
	//							Block::mDirt->getBlockState(BlockState::MappedType).set(data, DirtBlock::TYPE_NO_GRASS_SPREAD);
	//							blocks.setData(p, data);
	//						}
	//						else {
	//							blocks.setBlock(p, Block::mGrass->mID);
	//						}
	//					}
	//					else if (p.y > seaLevel + 3 + runDepth) {
	//						DataID d = 16;
	//						if (p.y < 64 || p.y > 127) {
	//							d = (byte)(~DyePowderItem::ORANGE & 0xf);
	//						}
	//						else if (claySurface) {
	//						}
	//						else {
	//							d = getBand(pos.x, p.y, pos.z);
	//						}
	//						if (d < 16) {
	//							blocks.setBlock(p, clay);
	//							blocks.setData(p, d);
	//						}
	//						else {
	//							blocks.setBlock(p, hardClay);
	//						}
	//					}
	//					else {
	//						blocks.setBlock(p, mTopMaterial);
	//						blocks.setData(p, mTopMaterialData);
	//						sandRun = true;
	//					}
	//				}
	//				else {
	//					blocks.setBlock(p, material);
	//					if (material == clay) {
	//						blocks.setData(p, (~DyePowderItem::ORANGE & 0xf));
	//					}
	//				}
	//			}
	//			else if (run > 0) {
	//				run--;

	//				if (sandRun) {
	//					blocks.setBlock(p, clay);
	//					blocks.setData(p, (~DyePowderItem::ORANGE & 0xf));
	//				}
	//				else {
	//					DataID d = getBand(pos.x, p.y, pos.z);
	//					if (d < 16) {
	//						blocks.setBlock(p, clay);
	//						blocks.setData(p, d);
	//					}
	//					else {
	//						blocks.setBlock(p, hardClay);
	//					}
	//				}
	//			}

	//			++clayDepth;
	//		}
	//	}
	//}
}

// TODO: rherlitz
void MesaBiome::generateBands(RandomSeed seed) {
	//memset(clayBands, 16, sizeof(clayBands));
	//Random random(seed);
	//clayBandsOffsetNoise = make_unique<PerlinSimplexNoise>(random, 1);

	//for (int i = 0; i < 64; ++i) {
	//	i += random.nextInt(5) + 1;
	//	if(i < 64) {
	//		clayBands[i] = (unsigned char)(~DyePowderItem::ORANGE & 0xf);
	//	}
	//}

	//int yellowBands = random.nextInt(4) + 2;

	//for (auto i : range(yellowBands)) {
	//	UNUSED_VARIABLE(i);
	//	int w = random.nextInt(3) + 1;
	//	int s = random.nextInt(64);

	//	for(int p = 0; (s + p) < 64 && p < w; p++) {
	//		clayBands[s + p] = (unsigned char)(~DyePowderItem::YELLOW & 0xf);
	//	}
	//}
	//int brownBands = random.nextInt(4) + 2;

	//for (auto i : range(brownBands)) {
	//	UNUSED_VARIABLE(i);
	//	int w = random.nextInt(3) + 2;
	//	int s = random.nextInt(64);

	//	for(int p = 0; (s + p) < 64 && p < w; p++) {
	//		clayBands[s + p] = (unsigned char)(~DyePowderItem::BROWN & 0xf);
	//	}
	//}
	//int redBands = random.nextInt(4) + 2;

	//for (auto i : range(redBands)) {
	//	UNUSED_VARIABLE(i);
	//	int w = random.nextInt(3) + 1;
	//	int s = random.nextInt(64);

	//	for(int p = 0; (s + p) < 64 && p < w; p++) {
	//		clayBands[s + p] = (unsigned char)(~DyePowderItem::RED & 0xf);
	//	}
	//}
	//int whiteBands = random.nextInt(3) + 3;
	//int s = 0;

	//for (auto i : range(whiteBands)) {
	//	UNUSED_VARIABLE(i);
	//	int w = 1;
	//	s += random.nextInt(16) + 4;

	//	for(int p = 0; (s + p) < 64 && p < w; p++) {
	//		clayBands[s + p] = (unsigned char)(~DyePowderItem::WHITE & 0xf);
	//		if(s + p > 1 && random.nextBoolean()) {
	//			clayBands[s + p - 1] = (unsigned char)(~DyePowderItem::SILVER & 0xf);
	//		}
	//		if(s + p < 63 && random.nextBoolean()) {
	//			clayBands[s + p + 1] = (unsigned char)(~DyePowderItem::SILVER & 0xf);
	//		}
	//	}
	//}
}

unsigned char MesaBiome::getBand(int worldX, int y, int worldZ) {
	int offset = (int)nearbyint(clayBandsOffsetNoise->getValue(worldX * 1.0f / 512.0f, worldZ * 1.0f / 512.0f) * 2.0f);
	return clayBands[(y + offset + 64) % 64];
}

Unique<Biome> MesaBiome::createMutatedCopy(int id) {
	bool pillars = mId == Biome::mesa->mId;

	auto mutatedBiome = make_unique<MesaBiome>(id, pillars, hasForest);

	if(!pillars) {
		mutatedBiome->setDepthAndScale(HEIGHTS_MOUNTAINS);
		mutatedBiome->setName(mName + " M");
	}
	else {
		mutatedBiome->setName(mName + " (Bryce)");
	}
	mutatedBiome->setColor(mDebugMapColor, true);

	return std::move(mutatedBiome);
}

void MesaBiome::Decorator::decorateOres(BlockSource* source, Random& random, const BlockPos& origin) {
	BiomeDecorator::decorateOres(source, random, origin);

	//additional gold
	decorateDepthSpan(source, random, origin, 20, goldOreFeature, 32, 80);
}
