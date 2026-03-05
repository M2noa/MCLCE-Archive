#include "Dungeons.h"
#include "world/level/biome/TaigaBiome.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/BlockPos.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/BlockSource.h"
#include "world/level/block/DoublePlantBlock.h"
#include "world/level/levelgen/feature/DoublePlantFeature.h"
#include "world/level/block/DirtBlock.h"
#include "world/entity/EntityTypes.h"

TaigaBiome::TaigaBiome(int id, int type) :
	Biome(id, Biome::BiomeType::Taiga)
	, type(type) {
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Wolf, 8, 4, 4));
	decorator->treeCount = 10;
	if(type == MEGA || type == MEGA_SPRUCE) {
		decorator->grassCount = 7;
		decorator->deadBushCount = 1;
		decorator->mushroomCount = 3;
	}
	else {
		decorator->grassCount = 1;
		decorator->mushroomCount = 1;
		_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Rabbit, 4, 2, 3));
	}
}

const FeaturePtr& TaigaBiome::getTreeFeature(Random* random){
	if((type == MEGA || type == MEGA_SPRUCE) && random->nextInt(3) == 0) {
		if(type == MEGA_SPRUCE || random->nextInt(13) == 0) {
			return decorator->megaSpruceTreeFeature;
		}
		return decorator->megaPineTreeFeature;
	}
	if(random->nextInt(3) == 0) {
		return decorator->pineTreeFeature;
	}
	return decorator->spruceTreeFeature;
}

const FeaturePtr& TaigaBiome::getGrassFeature(Random* random){
	if(random->nextInt(5) > 0) {
		return decorator->fernFeature;
	}
	return decorator->tallGrassFeature;
}

void TaigaBiome::decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float limit){
	if (type == MEGA || type == MEGA_SPRUCE) {
		int stoneCount = random.nextInt(3);

		for (int i = 0; i < stoneCount; i++) {
			const int randZ = origin.x + random.nextInt(16);
			const int randX = origin.z + random.nextInt(16);
			BlockPos p = BlockPos(randX, 0, randZ);

			p.y = source->getHeightmap(p);

			decorator->forestRockFeature->place(*source, p, random);
		}
	}

	DataID fernID = enum_cast(DoublePlantType::Fern);

	const int randZ = origin.x + random.nextInt(16);
	const int randX = origin.z + random.nextInt(16);
	BlockPos p = BlockPos(randX, 0, randZ);

	auto pos = source->getHeightmapPos(p);

	decorator->doublePlantFeature->place(*source, pos, random, fernID);

	Biome::decorate(source, random, origin, legacy, limit);
}

void TaigaBiome::buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) {
	if(type == MEGA || type == MEGA_SPRUCE) {
		mTopMaterial = Block::mGrass->mID;
		mTopMaterialData = 0;
		mMaterial = Block::mDirt->mID;
		if(depthValue > 1.75) {
			mTopMaterial = Block::mDirt->mID;
			Block::mDirt->getBlockState(BlockState::MappedType).set(mTopMaterialData, DirtBlock::TYPE_NO_GRASS_SPREAD);
		}
		else if(depthValue > -.95) {
			// Podzol is it's own type now instead of a dirt type
			mTopMaterial = Block::mPodzol->mID;
			mTopMaterialData = 0;
		}
	}
	buildSurfaceAtDefault(random, blocks, pos, depthValue, seaLevel);	// skip super
}

Unique<Biome> TaigaBiome::createMutatedCopy(int id) {
	if(mId == Biome::redwoodTaiga->mId) {
		auto m = make_unique<TaigaBiome>(id, MEGA_SPRUCE);
		m->setColor(0x596651, true);
		m->setName("Mega Spruce Taiga");
		m->setLeafColor(0x4EBA31);
		m->setTemperatureAndDownfall(0.25f, 0.8f);
		m->setDepthAndScale(BiomeHeight(mDepth, mScale));

		return std::move(m);
	}
	return Biome::createMutatedCopy(id);
}
