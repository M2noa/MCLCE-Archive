#include "Dungeons.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/levelgen/feature/ClayFeature.h"
#include "world/level/levelgen/feature/SandFeature.h"
#include "world/level/levelgen/feature/OreFeature.h"
#include "world/level/block/OldLeafBlock.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/block/TallGrass.h"
#include "world/level/block/StoneBlock.h"
#include "world/level/levelgen/feature/FlowerFeature.h"
#include "world/level/levelgen/feature/HugeMushroomFeature.h"
#include "world/level/levelgen/feature/ReedsFeature.h"
#include "world/level/levelgen/feature/CactusFeature.h"
#include "world/level/levelgen/feature/WaterlilyFeature.h"
#include "world/level/levelgen/feature/PumpkinFeature.h"
#include "world/level/levelgen/feature/SpringFeature.h"
#include "world/level/levelgen/feature/DeadBushFeature.h"
#include "world/level/levelgen/feature/DesertWellFeature.h"
#include "world/level/levelgen/feature/OakFeature.h"
#include "world/level/levelgen/feature/SpruceFeature.h"
#include "world/level/levelgen/feature/PineFeature.h"
#include "world/level/levelgen/feature/BirchFeature.h"
#include "world/level/levelgen/feature/RoofTreeFeature.h"
#include "world/level/levelgen/feature/DoublePlantFeature.h"
#include "world/level/levelgen/feature/MegaPineTreeFeature.h"
#include "world/level/levelgen/feature/TallGrassFeature.h"
#include "world/level/levelgen/feature/FancyTreeFeature.h"
#include "world/level/levelgen/feature/BlockBlobFeature.h"
#include "world/level/levelgen/feature/SwampTreeFeature.h"
#include "world/level/levelgen/feature/IceSpikeFeature.h"
#include "world/level/levelgen/feature/IcePatchFeature.h"
#include "world/level/levelgen/feature/GroundBushFeature.h"
#include "world/level/levelgen/feature/JungleTreeFeature.h"
#include "world/level/levelgen/feature/MegaJungleTreeFeature.h"
#include "world/level/levelgen/feature/MelonFeature.h"
#include "world/level/levelgen/feature/VinesFeature.h"
#include "world/level/levelgen/feature/SavannaTreeFeature.h"
#include "world/level/biome/Biome.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/FoliageColor.h"
#include "world/level/BlockPos.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"

const float BiomeDecorator::SNOW_CUTOFF = 0.5f;
const float BiomeDecorator::SNOW_SCALE = 0.3f;

char BiomeDecorator::gaussianKernel[5][5] {
	{
		2, 7, 12, 7, 2
	},
	{
		7, 22, 34, 22, 7
	},
	{
		12, 34, 0, 34, 12
	},
	{
		7, 22, 34, 22, 7
	},
	{
		2, 7, 12, 7, 2
	}
};

BiomeDecorator::BiomeDecorator() {
	//imported from Java 1.9 21/08/2015
	static const int
		dirtSize = 33,
		gravelSize = 33,
		graniteSize = 33,
		dioriteSize = 33,
		andesiteSize = 33,
		coalSize = 17,
		ironSize = 9,
		goldSize = 9,
		redstoneSize = 8,
		diamondSize = 8,
		lapisSize = 7;

	clayFeature = make_unique<ClayFeature>(4);
	sandFeature = make_unique<SandFeature>(Block::mSand->mID, 7);
	gravelFeature = make_unique<SandFeature>(Block::mGravel->mID, 6);
	dirtOreFeature = make_unique<OreFeature>(Block::mDirt->mID, dirtSize);
	gravelOreFeature = make_unique<OreFeature>(Block::mGravel->mID, gravelSize);
	graniteFeature = make_unique<OreFeature>(Block::mStone->mID, enum_cast(StoneBlock::StoneType::Granite), graniteSize);
	dioriteFeature = make_unique<OreFeature>(Block::mStone->mID, enum_cast(StoneBlock::StoneType::Diorite), dioriteSize);
	andesiteFeature = make_unique<OreFeature>(Block::mStone->mID, enum_cast(StoneBlock::StoneType::Andesite), andesiteSize);
	coalOreFeature = make_unique<OreFeature>(Block::mCoalOre->mID, coalSize);
	ironOreFeature = make_unique<OreFeature>(Block::mIronOre->mID, ironSize);
	goldOreFeature = make_unique<OreFeature>(Block::mGoldOre->mID, goldSize);
	redStoneOreFeature = make_unique<OreFeature>(Block::mRedStoneOre->mID, redstoneSize);
	diamondOreFeature = make_unique<OreFeature>(Block::mDiamondOre->mID, diamondSize);
	lapisOreFeature = make_unique<OreFeature>(Block::mLapisOre->mID, lapisSize);
	flowerFeature = make_unique<FlowerFeature>(Block::mYellowFlower->mID);
	brownMushroomFeature = make_unique<FlowerFeature>(Block::mBrownMushroom->mID);
	redMushroomFeature = make_unique<FlowerFeature>(Block::mRedMushroom->mID);
	hugeMushroomFeature = make_unique<HugeMushroomFeature>();
	reedsFeature = make_unique<ReedsFeature>();
	cactusFeature = make_unique<CactusFeature>();
	waterlilyFeature = make_unique<WaterlilyFeature>();
	pumpkinFeature = make_unique<PumpkinFeature>();
	waterSpringFeature = make_unique<SpringFeature>(Block::mFlowingWater->mID);
	lavaSpringFeature = make_unique<SpringFeature>(Block::mFlowingLava->mID);
	deadBushFeature = make_unique<DeadBushFeature>();
	desertWellFeature = make_unique<DesertWellFeature>();
	oakFeature = make_unique<OakFeature>();
	spruceTreeFeature = make_unique<SpruceFeature>();
	birchFeature = make_unique<BirchFeature>();
	roofTreeFeature = make_unique<RoofTreeFeature>(nullptr);
	doublePlantFeature = make_unique<DoublePlantFeature>();
	pineTreeFeature = make_unique<PineFeature>();
	megaPineTreeFeature = make_unique<MegaPineTreeFeature>(nullptr, false);
	megaSpruceTreeFeature = make_unique<MegaPineTreeFeature>(nullptr, true);
	tallGrassFeature = make_unique<TallGrassFeature>(Block::mTallgrass->mID, enum_cast(TallGrassType::Tall));
	fernFeature = make_unique<TallGrassFeature>(Block::mTallgrass->mID, enum_cast(TallGrassType::Fern));
	fancyTreeFeature = make_unique<FancyTreeFeature>();
	forestRockFeature = make_unique<BlockBlobFeature>(Block::mMossyCobblestone->mID, 0);
	swampTreeFeature = make_unique<SwampTreeFeature>();
	iceSpikeFeature = make_unique<IceSpikeFeature>();
	icePatchFeature = make_unique<IcePatchFeature>(4);
	jungleBushFeature = make_unique<GroundBushFeature>(enum_cast(OldLogBlock::LogType::Jungle), enum_cast(OldLeafBlock::LeafType::Jungle));
	jungleTreeFeature = make_unique<JungleTreeFeature>();
	megaJungleTreeFeature = make_unique<MegaJungleTreeFeature>(nullptr, 10, 20, enum_cast(OldLogBlock::LogType::Jungle), enum_cast(OldLeafBlock::LeafType::Jungle));
	melonFeature = make_unique<MelonFeature>();
	vinesFeature = make_unique<VinesFeature>();
	savannaTreeFeature = make_unique<SavannaTreeFeature>(nullptr);
	superBirchFeature = make_unique<BirchFeature>(nullptr, true);
}

BiomeDecorator::~BiomeDecorator() {

}

int BiomeDecorator::_getRandomHeight(int x, int z, BlockSource* source, Random& random, const BlockPos& origin){
	auto h = source->getHeightmap(x + origin.x, z + origin.z) * 2;
	return h ? random.nextInt(h) : 0;
}

void BiomeDecorator::_placeFeature(BlockSource* source, const FeaturePtr& feature, const BlockPos& origin, Random& random){
	if (source != nullptr) {
		feature->place(*source, origin, random);
	}
}

BlockPos BiomeDecorator::_getRandomSurfacePosition(BlockSource* region, const BlockPos& chunkMin, Random& random){
	
	const int randZ = chunkMin.z + random.nextInt(16);
	const int randX = chunkMin.x + random.nextInt(16);

	BlockPos p(randX, 0, randZ);

	int h = region->getHeightmap(p);
	p.y = h ? random.nextInt(h) : 0;

	return p;
}

BlockPos BiomeDecorator::_getRandomTreePosition(BlockSource* region, const BlockPos& chunkMin, Random& random) {
	const int randZ = chunkMin.z + 1 + random.nextInt(14);
	const int randX = chunkMin.x + 1 + random.nextInt(14);

	BlockPos p(randX, 0, randZ);
	p.y = region->getHeightmap(p);

	return p;
}

BlockPos BiomeDecorator::_getRandomSolidPosition(BlockSource* region, const BlockPos& chunkMin, Random& random){
	const int randZ = chunkMin.z + random.nextInt(16);
	const int randX = chunkMin.x + random.nextInt(16);

	BlockPos p(randX, 0, randZ);
	auto h = region->getAboveTopSolidBlock(p);
	p.y = h ? random.nextInt(h) : 0;

	return p;
}

void BiomeDecorator::decorate(BlockSource* source, Random& random, Biome* biome, const BlockPos& origin, bool legacy, float limit){

	// Implemented for correctness right now, we can replace most of the getHeightmapPos with getHeightmap
	// but right now it is more important with correctness
	decorateOres(source, random, origin);

	// Sand
	for (int i = 0; i < sandCount; i++) {
		const int x = origin.x + random.nextInt(16);
		const int z = origin.z + random.nextInt(16);
		const int y = source->getAboveTopSolidBlock(x, z);
		_placeFeature(source, sandFeature, BlockPos(x, y, z), random);
	}

	// Clay
	for (int i = 0; i < clayCount; i++) {
		const int x = origin.x + random.nextInt(16);
		const int z = origin.z + random.nextInt(16);
		const int y = source->getAboveTopSolidBlock(x, z);
		_placeFeature(source, clayFeature, BlockPos(x, y, z), random);
	}

	// Gravel
	for (int i = 0; i < gravelCount; i++) {
		const int x = origin.x + random.nextInt(16);
		const int z = origin.z + random.nextInt(16);
		const int y = source->getAboveTopSolidBlock(x, z);
		_placeFeature(source, gravelFeature, BlockPos(x, y, z), random);
	}

	int grassCount = this->grassCount;
	int forests = 0;
	if (treeCount > 0) {
		if (random.nextInt(10) == 0) {
			forests = (int)random.nextFloat(treeCount);
			grassCount = Biome::plains->decorator->grassCount;
		}
		else {
			if (treeCount >= 1.f) {
				forests = (int)treeCount + std::max(0, random.nextInt(10) - 7);
			}
			else if (treeCount > 0) {
				forests = random.nextFloat() < treeCount ? 1 : 0;
			}
			else{
				DEBUG_FAIL("invalid tree count");
			}
		}
	}

	// Trees
	for (int i = 0; i < forests; i++) {

		auto& tree = biome->getTreeFeature(&random);
		//tree.init();
		if (tree != nullptr) {
			auto pos = _getRandomTreePosition(source, origin, random);
			_placeFeature(
				source,
				tree,
				pos, random);

			//place additional tall grass around trees
			if ( !legacy ) {
				TallGrassFeature(Block::mTallgrass->mID, enum_cast(TallGrassType::Tall)).place(*source, pos, random, 23, 3);
			}
		}
	}

	// Huge Mushrooms
	for (int i = 0; i < hugeMushrooms; i++) {
		const int x = origin.x + random.nextInt(16);
		const int z = origin.z + random.nextInt(16);
		const int y = source->getAboveTopSolidBlock(x, z);
		_placeFeature(source, hugeMushroomFeature, BlockPos(x, y, z), random);
	}

	// FLowers
	for (int i = 0; i < flowerCount; i++) {
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		const int y = random.nextInt(source->getHeightmapPos(origin.offset(x, 0, z)).y + 32);

		const BlockPos pos = origin.offset(x, y, z);
		auto flower = biome->getRandomFlowerTypeAndData(random, pos);
		if(source->getMaterial(pos) != Material::getMaterial(MaterialType::Air)) {
			flowerFeature->placeFlower(*source, pos, flower, random);
		}
	}

	// Tall grass
	for(int i = 0; i < grassCount && !legacy; i++) {
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		const int y = _getRandomHeight(x, z, source, random, origin);
		auto pos = origin.offset(x, y, z);

		_placeFeature(source, biome->getGrassFeature(&random), pos, random);
	}

	// Dead bush
	for (auto i : range(deadBushCount)) {
		UNUSED_VARIABLE(i);
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		const int y = _getRandomHeight(x, z, source, random, origin);
		auto pos = origin.offset(x, y, z);
		_placeFeature(source, deadBushFeature, pos, random);
	}

	// Water lilies
	for (auto i : range(waterlilyCount)) {
		UNUSED_VARIABLE(i);
		int x = random.nextInt(16);
		int z = random.nextInt(16);
		int y = _getRandomHeight(x, z, source, random, origin);

		BlockPos pos = origin.offset(x, y, z);

		while(pos.y > 0) {
			const BlockPos below = pos.below();
			if(!source->isEmptyBlock(below)) {
				break;
			}

			pos = below;
		}
		_placeFeature(source, waterlilyFeature, pos, random);
	}

	// Mushrooms (small)
	for (auto i : range(mushroomCount)) {
		UNUSED_VARIABLE(i);
		if(random.nextInt(4) == 0) {
			const int x = random.nextInt(16);
			const int z = random.nextInt(16);
			const BlockPos pos = source->getHeightmapPos(origin.offset(x, 0, z));
			_placeFeature(source, brownMushroomFeature, pos, random);
		}

		if(random.nextInt(8) == 0) {
			const int x = random.nextInt(16);
			const int z = random.nextInt(16);
			const int y = _getRandomHeight(x, z, source, random, origin);
			const BlockPos offset = origin.offset(x, y, z);
			_placeFeature(source, redMushroomFeature, offset, random);
		}
	}

	// Extra brown mushroom (see java)
	if(random.nextInt(4) == 0) {
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		const int y = _getRandomHeight(x, z, source, random, origin);
		BlockPos offset = origin.offset(x, y, z);
		_placeFeature(source, brownMushroomFeature, offset, random);
	}

	// Extra red mushroom (see java)
	if(random.nextInt(8) == 0) {
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		const int y = _getRandomHeight(x, z, source, random, origin);
		BlockPos offset = origin.offset(x, y, z);
		_placeFeature(source, redMushroomFeature, offset, random);
	}

	// Reeds
	for (auto i : range(reedsCount)) {
		UNUSED_VARIABLE(i);
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		const int y = _getRandomHeight(x, z, source, random, origin);
		BlockPos offset = origin.offset(x, y, z);
		_placeFeature(source, reedsFeature, offset, random);
	}

	// Reeds 2, supposed to be added twice?? (see java)
	for (auto i : range(10)) {
		UNUSED_VARIABLE(i);
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		const int y = _getRandomHeight(x, z, source, random, origin);
		BlockPos offset = origin.offset(x, y, z);
		_placeFeature(source, reedsFeature, offset, random);
	}

	// Pumpkins
	if (random.nextInt(32) == 0) {
		_placeFeature(source, pumpkinFeature, _getRandomSurfacePosition(source, origin, random), random);
	}

	// Cactus
	for (auto i : range(cactusCount)) {
		UNUSED_VARIABLE(i);
		const int x = random.nextInt(16);
		const int z = random.nextInt(16);
		const int y = _getRandomHeight(x, z, source, random, origin);
		BlockPos offset = origin.offset(x, y, z);
		_placeFeature(source, cactusFeature, offset, random);
	}

	//less springs in the end
	if (Vec3(origin).length() > limit && random.nextInt(4) != 0) {
		liquids = false;
	}

	// Springs
	if (liquids) {
		for (int i = 0; i < 50; i++) {
			int randZ = random.nextInt(16);
			int randYParam = random.nextInt(LEVEL_GEN_HEIGHT - 8) + 8;
			int randY = random.nextInt(randYParam);
			int randX = random.nextInt(16);
			const BlockPos pos = origin.offset(randX, randY, randZ);
			_placeFeature(source, waterSpringFeature, pos, random);
		}

		for (int i = 0; i < 20; i++) {
			int randZ = random.nextInt(16);
			int randYParam1 = random.nextInt(LEVEL_GEN_HEIGHT - 16) + 8;
			int randYParam2 = random.nextInt(randYParam1) + 8;
			int randY = random.nextInt(randYParam2);
			int randX = random.nextInt(16);
			const BlockPos pos = origin.offset(randX, randY, randZ);
			_placeFeature(source, lavaSpringFeature, pos, random);
		}
	}
}

void BiomeDecorator::decorateDepthSpan(BlockSource* region, Random& random, const BlockPos& origin, int count, FeaturePtr& feature, int y0, int y1){
	if(feature == nullptr) {
		return;
	}
	for (auto i : range(count)) {
		UNUSED_VARIABLE(i);
		const int randZ1 = random.nextInt(16);
		const int randY1 = random.nextInt(y1 - y0) + y0;
		const int randX1 = random.nextInt(16);
		const BlockPos pos = origin.offset(randX1, randY1, randZ1);

 		_placeFeature(region, feature, pos, random);
	}
}

void BiomeDecorator::decorateDepthAverage(BlockSource* region, Random& random, const BlockPos& origin, int count, FeaturePtr& feature, int yMid, int ySpan){
	if(feature == nullptr) {
		return;
	}

	for (auto i : range(count)) {
		UNUSED_VARIABLE(i);

		const int randZ = random.nextInt(16);
		const int randY0 = random.nextInt(ySpan);
		const int randY1 = random.nextInt(ySpan) + yMid - ySpan;
		const int randX = random.nextInt(16);
		const BlockPos pos = origin.offset(randX, randY0 + randY1, randZ);

		_placeFeature(region, feature, pos, random);
	}
}

void BiomeDecorator::decorateOres(BlockSource* source, Random& random, const BlockPos& origin){
	//imported from Java 1.9 21/08/2015
	static const int
		dirtCount = 10,
		gravelCount = 8,
		graniteCount = 10,
		dioriteCount = 10,
		andesiteCount = 10,
		coalCount = 20,
		ironCount = 20,
		goldCount = 2,
		redstoneCount = 8,
		diamondCount = 1,
		lapisCount = 1;

	decorateDepthSpan(source, random, origin, dirtCount, dirtOreFeature, 0, LEVEL_GEN_HEIGHT);

	decorateDepthSpan(source, random, origin, gravelCount, gravelOreFeature, 0, LEVEL_GEN_HEIGHT);

	//PE unique feature: 1/16 chunks have A LOT of gravel!
	if (random.nextInt(16) == 0 ) {
		decorateDepthSpan(source, random, origin, 80, gravelOreFeature, 0, 50);
	}

	decorateDepthSpan(source, random, origin, dioriteCount, dioriteFeature, 0, 80);
	decorateDepthSpan(source, random, origin, graniteCount, graniteFeature, 0, 80);
	decorateDepthSpan(source, random, origin, andesiteCount, andesiteFeature, 0, 80);
	decorateDepthSpan(source, random, origin, coalCount, coalOreFeature, 0, LEVEL_GEN_HEIGHT);
	decorateDepthSpan(source, random, origin, ironCount, ironOreFeature, 0, 64);
	decorateDepthSpan(source, random, origin, goldCount, goldOreFeature, 0, 32);
	decorateDepthSpan(source, random, origin, redstoneCount, redStoneOreFeature, 0, 16);
	decorateDepthSpan(source, random, origin, diamondCount, diamondOreFeature, 0, 16);
	decorateDepthAverage(source, random, origin, lapisCount, lapisOreFeature, 16, 16);
}
