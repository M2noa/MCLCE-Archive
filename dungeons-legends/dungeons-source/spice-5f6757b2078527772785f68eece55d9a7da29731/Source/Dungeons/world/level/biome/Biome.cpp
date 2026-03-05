#include "Dungeons.h"

#include "world/level/biome/BiomeInclude.h"

#include "world/level/levelgen/feature/OakFeature.h"
#include "world/level/levelgen/feature/TallGrassFeature.h"
#include "world/level/storage/LevelStorage.h"

//#include "world/entity/ParticleType.h"
#include "world/level/block/TallGrass.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/biome/MobSpawnUtils.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/FoliageColor.h"
#include "world/level/levelgen/feature/FlowerFeature.h"
#include "world/level/levelgen/feature/DoublePlantFeature.h"
#include "world/entity/EntityTypes.h"
#include "world/entity/EntityClassTree.h"
#include "world/level/block/BlockVolume.h"
#include "world/level/Seasons.h"
#include "util/KeyValueInput.h"
#include "Core/Debug/Log.h"
#include "nbt/NbtIo.h"
#include "nbt/ListTag.h"
#include "world/level/block/BlockVolume.h"
#include "world/level/ChunkBlockPos.h"

const float Biome::BiomeHeight::MIN_DEPTH = -2.0f;

const Color Biome::DEFAULT_WATER_COLOR = Color::fromARGB(0x0098e8ff);
const Color Biome::DEFAULT_UNDERWATER_COLOR = Color::fromARGB(0x00050532);

const float Biome::RAIN_TEMP_THRESHOLD = 0.15f;

const Biome::BiomeHeight Biome::HEIGHTS_DEFAULT(0.1f, 0.2f);
const Biome::BiomeHeight Biome::HEIGHTS_RIVER(-.5f, 0.0f);
const Biome::BiomeHeight Biome::HEIGHTS_OCEAN(-1.0f, 0.1f);
const Biome::BiomeHeight Biome::HEIGHTS_DEEP_OCEAN(-1.8f, 0.1f);
const Biome::BiomeHeight Biome::HEIGHTS_LOWLANDS(0.125f, 0.05f);
const Biome::BiomeHeight Biome::HEIGHTS_TAIGA(0.2f, 0.2f);
const Biome::BiomeHeight Biome::HEIGHTS_MOUNTAINS(0.45f, 0.3f);
const Biome::BiomeHeight Biome::HEIGHTS_HIGHLANDS(1.5f, 0.025f);
const Biome::BiomeHeight Biome::HEIGHTS_EXTREME(1.0f, 0.5f);
const Biome::BiomeHeight Biome::HEIGHTS_BEACH(0.0f, 0.025f);
const Biome::BiomeHeight Biome::HEIGHTS_STONE_BEACH(0.1f, 0.8f);
const Biome::BiomeHeight Biome::HEIGHTS_MUSHROOM(0.2f, 0.3f);
const Biome::BiomeHeight Biome::HEIGHTS_SWAMPLAND(-.2f, 0.1f);

BiomePtr Biome::ocean,
Biome::plains,
Biome::desert,
Biome::extremeHills,
Biome::forest,
Biome::taiga,
Biome::swampland,
Biome::river,
Biome::hell,
Biome::sky,
Biome::frozenOcean,
Biome::frozenRiver,
Biome::iceFlats,
Biome::iceMountains,
Biome::mushroomIsland,
Biome::mushroomIslandShore,
Biome::beaches,
Biome::desertHills,
Biome::forestHills,
Biome::taigaHills,
Biome::smallerExtremeHills,

Biome::jungle,
Biome::jungleHills,
Biome::jungleEdge,

Biome::deepOcean,
Biome::stoneBeach,
Biome::coldBeach,

Biome::birchForest,
Biome::birchForestHills,
Biome::roofedForest,

Biome::taigaCold,
Biome::taigaColdHills,
Biome::redwoodTaiga,
Biome::redwoodTaigaHills,

Biome::extremeHillsWithTrees,

Biome::savanna,
Biome::savannaRock,

Biome::mesa,
Biome::mesaRock,
Biome::mesaClearRock,

Biome::plainsMutated,
Biome::desertMutated,
Biome::forestMutated,
Biome::taigaMutated,
Biome::swamplandMutated,
Biome::iceFlatsMutated,
Biome::jungleMutated,
Biome::jungleEdgeMutated,
Biome::taigaColdMutated,
Biome::savannaMutated,
Biome::savannaRockMutated,
Biome::mesaMutated,
Biome::mesaRockMutated,
Biome::mesaClearRockMutated,
Biome::birchForestMutated,
Biome::birchForestHillsMutated,
Biome::roofedForestMutated,
Biome::redwoodTaigaMutated,
Biome::extremeHillsMutated,
Biome::extremeHillsWithTreesMutated,
Biome::redwoodTaigaHillsMutated;
Biome* Biome::DEFAULT;

/*static*/
MobList Biome::_emptyMobList;
int Biome::defaultTotalEnemyWeight = 0;
int Biome::defaultTotalFriendlyWeight = 0;

const MobList Biome::EMPTY_MOBLIST = MobList();

/*static*/
Biome* Biome::mBiomes[Biome::BIOMES_COUNT] {
	nullptr
};

void Biome::initBiomes() {
	(ocean = make_unique<OceanBiome>(0))->setColor(0x000070).setName("Ocean").setDepthAndScale(HEIGHTS_OCEAN);
	(plains = make_unique<PlainsBiome>(1))->setColor(0x8db360).setName("Plains");
	(desert = make_unique<DesertBiome>(2))->setColor(0xFA9418).setName("Desert").setNoRain().setTemperatureAndDownfall(2, 0).setDepthAndScale(HEIGHTS_LOWLANDS);
	(extremeHills = make_unique<ExtremeHillsBiome>(3, false))->setColor(0x606060).setName("Extreme Hills").setDepthAndScale(HEIGHTS_EXTREME).setTemperatureAndDownfall(0.2f, 0.3f);
	(forest = make_unique<ForestBiome>(4, ForestBiome::TYPE_NORMAL))->setColor(0x056621).setName("Forest");
	(taiga = make_unique<TaigaBiome>(5, TaigaBiome::NORMAL))->setColor(0x0b6659).setName("Taiga").setLeafColor(0x4EBA31).setTemperatureAndDownfall(0.25f, 0.8f);
	(swampland = make_unique<SwampBiome>(6))->setColor(0x07F9B2).setName("Swampland").setLeafColor(0x8BAF48).setDepthAndScale(HEIGHTS_SWAMPLAND).setTemperatureAndDownfall(0.8f, 0.5f);
	(river = make_unique<RiverBiome>(7))->setColor(0x0000ff).setName("River").setDepthAndScale(HEIGHTS_RIVER);
	(hell = make_unique<HellBiome>(8))->setColor(0xff0000).setName("Hell").setNoRain().setTemperatureAndDownfall(2, 0);
	(sky = make_unique<TheEndBiome>(9))->setColor(0x8080ff).setName("Sky").setNoRain();
	(frozenOcean = make_unique<OceanBiome>(10))->setColor(0x9090a0).setName("FrozenOcean").setSnowAccumulation(1.0f / 8.0f, 2.0f / 8.0f).setDepthAndScale(HEIGHTS_OCEAN).setTemperatureAndDownfall(0, 0.5f);
	(frozenRiver = make_unique<RiverBiome>(11))->setColor(0xa0a0ff).setName("FrozenRiver").setSnowAccumulation(1.0f / 8.0f, 2.0f / 8.0f).setDepthAndScale(HEIGHTS_RIVER).setTemperatureAndDownfall(0, 0.5f);
	(iceFlats = make_unique<IceBiome>(12, false))->setColor(0xffffff).setName("Ice Plains").setSnowAccumulation(2.0f / 8.0f, 1.0f).setTemperatureAndDownfall(0, 0.5f).setDepthAndScale(HEIGHTS_LOWLANDS);
	(iceMountains = make_unique<IceBiome>(13, false))->setColor(0xa0a0a0).setName("Ice Mountains").setSnowAccumulation(3.0f / 8.0f, 1.5f).setDepthAndScale(HEIGHTS_MOUNTAINS).setTemperatureAndDownfall(0, 0.5f);
	(mushroomIsland = make_unique<MushroomIslandBiome>(14))->setColor(0xff00ff).setName("MushroomIsland").setTemperatureAndDownfall(0.9f, 1.0f).setDepthAndScale(HEIGHTS_MUSHROOM);
	(mushroomIslandShore = make_unique<MushroomIslandBiome>(15))->setColor(0xa000ff).setName("MushroomIslandShore").setTemperatureAndDownfall(0.9f, 1.0f).setDepthAndScale(HEIGHTS_BEACH);
	(beaches = make_unique<BeachBiome>(16))->setColor(0xfade55).setName("Beach").setTemperatureAndDownfall(0.8f, 0.4f).setDepthAndScale(HEIGHTS_BEACH);
	(desertHills = make_unique<DesertBiome>(17))->setColor(0xd25f12).setName("DesertHills").setNoRain().setTemperatureAndDownfall(2, 0).setDepthAndScale(HEIGHTS_MOUNTAINS);
	(forestHills = make_unique<ForestBiome>(18, ForestBiome::TYPE_NORMAL))->setColor(0x22551c).setName("ForestHills").setDepthAndScale(HEIGHTS_MOUNTAINS);
	(taigaHills = make_unique<TaigaBiome>(19, TaigaBiome::NORMAL))->setColor(0x163933).setName("TaigaHills").setLeafColor(0x4EBA31).setTemperatureAndDownfall(0.25f, 0.8f).setDepthAndScale(HEIGHTS_MOUNTAINS);
	(smallerExtremeHills = make_unique<ExtremeHillsBiome>(20, true))->setColor(0x72789a).setName("Extreme Hills Edge").setDepthAndScale(HEIGHTS_EXTREME.less()).setTemperatureAndDownfall(0.2f, 0.3f);

	(jungle = make_unique<JungleBiome>(21, false))->setColor(0x537b09).setName("Jungle").setLeafColor(0x537b09).setTemperatureAndDownfall(0.95f, 0.9f);
	(jungleHills = make_unique<JungleBiome>(22, false))->setColor(0x2c4205).setName("JungleHills").setLeafColor(0x537b09).setTemperatureAndDownfall(0.95f, 0.9f).setDepthAndScale(HEIGHTS_MOUNTAINS);
	(jungleEdge = make_unique<JungleBiome>(23, true))->setColor(0x628b17).setName("JungleEdge").setLeafColor(0x537b09).setTemperatureAndDownfall(0.95f, 0.8f);

	(deepOcean = make_unique<OceanBiome>(24))->setColor(0x000030).setName("Deep Ocean").setDepthAndScale(HEIGHTS_DEEP_OCEAN);
	(stoneBeach = make_unique<StoneBeachBiome>(25))->setColor(0xa2a284).setName("Stone Beach").setTemperatureAndDownfall(0.2f, 0.3f).setDepthAndScale(HEIGHTS_STONE_BEACH);
	(coldBeach = make_unique<BeachBiome>(26))->setColor(0xfaf0c0).setName("Cold Beach").setTemperatureAndDownfall(0.05f, 0.3f).setDepthAndScale(HEIGHTS_BEACH).setSnowAccumulation(1.0f / 8.0f, 2.0f / 8.0f);

	(birchForest = make_unique<ForestBiome>(27, ForestBiome::TYPE_BIRCH))->setName("Birch Forest").setColor(0x307444);
	(birchForestHills = make_unique<ForestBiome>(28, ForestBiome::TYPE_BIRCH))->setName("Birch Forest Hills").setColor(0x1f5f32).setDepthAndScale(HEIGHTS_MOUNTAINS);
	(roofedForest = make_unique<ForestBiome>(29, ForestBiome::TYPE_ROOFED))->setColor(0x40511a).setName("Roofed Forest");

	(taigaCold = make_unique<TaigaBiome>(30, TaigaBiome::NORMAL))->setColor(0x31554a).setName("Cold Taiga").setLeafColor(0x4EBA31).setSnowAccumulation(1.0f / 8.0f, 0.5f).setTemperatureAndDownfall(-0.5f, 0.4f).setDepthAndScale(HEIGHTS_TAIGA).setOddColor(0xffffff);
	(taigaColdHills = make_unique<TaigaBiome>(31, TaigaBiome::NORMAL))->setColor(0x243f36).setName("Cold Taiga Hills").setLeafColor(0x4EBA31).setSnowAccumulation(1.0f / 8.0f, 0.5f).setTemperatureAndDownfall(-0.5f, 0.4f).setDepthAndScale(HEIGHTS_MOUNTAINS).setOddColor(0xffffff);
	(redwoodTaiga = make_unique<TaigaBiome>(32, TaigaBiome::MEGA))->setColor(0x596651).setName("Mega Taiga").setLeafColor(0x4EBA31).setTemperatureAndDownfall(0.3f, 0.8f).setDepthAndScale(HEIGHTS_TAIGA);
	(redwoodTaigaHills = make_unique<TaigaBiome>(33, TaigaBiome::MEGA))->setColor(0x454f3e).setName("Mega Taiga Hills").setLeafColor(0x4EBA31).setTemperatureAndDownfall(0.3f, 0.8f).setDepthAndScale(HEIGHTS_MOUNTAINS);

	(extremeHillsWithTrees = make_unique<ExtremeHillsBiome>(34, true))->setColor(0x507050).setName("Extreme Hills+").setDepthAndScale(HEIGHTS_EXTREME).setTemperatureAndDownfall(0.2f, 0.3f);

	(savanna = make_unique<SavannaBiome>(35))->setColor(0xbdb25f).setName("Savanna").setTemperatureAndDownfall(1.2f, 0).setNoRain().setDepthAndScale(HEIGHTS_LOWLANDS);
	(savannaRock = make_unique<SavannaBiome>(36))->setColor(0xa79d64).setName("Savanna Plateau").setTemperatureAndDownfall(1.0f, 0).setNoRain().setDepthAndScale(HEIGHTS_HIGHLANDS);

	(mesa = make_unique<MesaBiome>(37, false, false))->setColor(0xd94515).setName("Mesa");
	(mesaRock = make_unique<MesaBiome>(38, false, true))->setColor(0xb09765).setName("Mesa Plateau F");
	(mesaClearRock = make_unique<MesaBiome>(39, false, false))->setColor(0xca8c65).setName("Mesa Plateau").setDepthAndScale(HEIGHTS_HIGHLANDS);

	plainsMutated = plains->createMutatedCopy();
	desertMutated = desert->createMutatedCopy();
	forestMutated = forest->createMutatedCopy();
	taigaMutated = taiga->createMutatedCopy();
	swamplandMutated = swampland->createMutatedCopy();
	iceFlatsMutated = iceFlats->createMutatedCopy();
	jungleMutated = jungle->createMutatedCopy();
	jungleEdgeMutated = jungleEdge->createMutatedCopy();
	taigaColdMutated = taigaCold->createMutatedCopy();
	savannaMutated = savanna->createMutatedCopy();
	savannaRockMutated = savannaRock->createMutatedCopy();
	mesaMutated = mesa->createMutatedCopy();
	mesaRockMutated = mesaRock->createMutatedCopy();
	mesaClearRockMutated = mesaClearRock->createMutatedCopy();
	birchForestMutated = birchForest->createMutatedCopy();
	birchForestHillsMutated = birchForestHills->createMutatedCopy();
	roofedForestMutated = roofedForest->createMutatedCopy();
	redwoodTaigaMutated = redwoodTaiga->createMutatedCopy();
	extremeHillsMutated = extremeHills->createMutatedCopy();
	extremeHillsWithTreesMutated = extremeHillsWithTrees->createMutatedCopy();
	redwoodTaigaHillsMutated = redwoodTaigaHills->createMutatedCopy(redwoodTaigaHills->mId + Biome::MUTATED_OFFSET);
	redwoodTaigaMutated->setName("Redwood Taiga Hills M");
	DEFAULT = plains.get();
}

Biome::Biome( int id, BiomeType biomeType, Unique<BiomeDecorator> customDecorator ) :
	mTopMaterial(((const Block*)Block::mGrass)->mID)
	, mTopMaterialData(0)
	, mMaterial(((const Block*)Block::mDirt)->mID)
	, mLeafColor(0x4EE031)
	, mId(id)
	, mBiomeType(biomeType)
	, mBiomeInfoNoise(make_unique<PerlinSimplexNoise>(2345L, 1))
	, decorator( customDecorator ? std::move(customDecorator) : make_unique<BiomeDecorator>())
	, mMinSnowLevel(0)
	, mMaxSnowLevel(1.0f / 8.0f) {
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Sheep, 12, 2, 3));
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Pig, 10, 1, 3));
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Chicken, 10, 2, 4));
	_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Cow, 8, 2, 3));

	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Spider, 100, 1, 1));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Zombie, 80, 2, 4, nullptr, MobSpawnUtils::spawnZombie));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Skeleton, 100, 1, 2));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Creeper, 100, 1, 2));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Slime, 100, 1, 1));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::EnderMan, 10, 1, 2));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Witch, 5, 1, 1));
	_enemies.insert(_enemies.end(), MobSpawnerData(EntityType::Bat, 10, 2, 2));
	
	setDepthAndScale(HEIGHTS_DEFAULT);

	//
	// Sum up the weights
	//
	defaultTotalEnemyWeight = 0;

	for (MobList::const_iterator cit = _enemies.begin(); cit != _enemies.end(); ++cit) {
		defaultTotalEnemyWeight += cit->getWeight();
	}

	defaultTotalFriendlyWeight = 0;

	for (MobList::const_iterator cit = _friendlies.begin(); cit != _friendlies.end(); ++cit) {
		defaultTotalFriendlyWeight += cit->getWeight();
	}

	mBiomes[id] = this;
}

Biome::~Biome() {

}

Biome& Biome::setName( const std::string& name ){
	mName = name;
	return *this;
}

Biome& Biome::setLeafColor( int leafColor ){
	mLeafColor = leafColor;
	return *this;
}

Biome& Biome::setColor( int color ){
	return setColor(color, false);
}

Biome& Biome::setColor(int color, bool oddColor) {
//  if((color & 0xff) != 0xff)
//      color = (color << 8) | 0xff;
	//color = color | 0xff000000;
	int debugOddColor = (color & 0xfefefe);
	color = 0xff000000 | ((color & 0x00ff0000) >> 16) | ((color & 0x0000ff00)) | ((color & 0x000000ff) << 16);
	mDebugMapColor = color;
	if(oddColor) {
		mDebugMapOddColor = debugOddColor >> 1;
	}
	else {
		mDebugMapOddColor = color;
	}
	return *this;
}

Biome& Biome::setOddColor( int color ) {
	mDebugMapOddColor = color;
	return *this;
}

Biome& Biome::setTemperatureAndDownfall(float temperature, float downfall) {
	mTemperature = temperature;
	mDownfall = downfall;

	return *this;
}

Biome& Biome::clearMobs( bool friendlies, bool waterFriendlies, bool enemies){
	if (friendlies) {
		_friendlies.clear();
	}
	if (waterFriendlies) {
		_waterFriendlies.clear();
	}
	if (enemies) {
		_enemies.clear();
	}
	return *this;
}

Biome& Biome::setDepthAndScale(const BiomeHeight& heightData) {
	mDepth = heightData.depth;
	mScale = heightData.scale;
	return *this;
}

Biome& Biome::setNoRain() {
	mRain = false;
	return *this;
}

Biome& Biome::setSnowCovered() {
	setSnowAccumulation(1.0f / 8.0f, 0.5f);
	return *this;
}

Biome& Biome::setSnowAccumulation(float minSnowLevel, float maxSnowLevel) {
	DEBUG_ASSERT(minSnowLevel <= maxSnowLevel, "minSnowLevel should be <= maxSnowLevel");
	mSnowAccumulation = minSnowLevel;
	mMinSnowLevel = minSnowLevel;
	mMaxSnowLevel = maxSnowLevel;
	if (minSnowLevel > 0) {
		_friendlies.insert(_friendlies.end(), MobSpawnerData(EntityType::Rabbit, 4, 2, 3));
	}
	return *this;
}

int Biome::getSnowAccumulationLayers() const {
	return static_cast<int>(mSnowAccumulation * 8.0f);
}

const FeaturePtr& Biome::getTreeFeature( Random* random ) {
	if (random->nextInt(10) == 0) {
		return decorator->fancyTreeFeature;
	}
	return decorator->oakFeature;
}

const FeaturePtr& Biome::getGrassFeature( Random* random ) {
	return decorator->tallGrassFeature;
}

Biome* Biome::getBiome( int id) {
	return getBiome(id, nullptr);
}

Biome* Biome::getBiome(int id, Biome* defaultBiome) {

	if(id < 0 || id > BIOMES_COUNT) {
		//LOGW("Biome ID is out of bounds: %d, defaulting to 0 (Ocean)\n", id);
		return ocean.get();
	}
	Biome* biome = mBiomes[id];
	if(biome == nullptr) {
		return defaultBiome;
	}
	return biome;
}

float Biome::adjustScale(float scale){
	return scale;
}

float Biome::adjustDepth( float depth ){
	return depth;
}

Color Biome::getSkyColor( float temp ){
	temp /= 3.f;
	if (temp < -1) {
		temp = -1;
	}
	if (temp > 1) {
		temp = 1;
	}
	return Color::fromHSB(224.f / 360.0f - temp * 0.05f, 0.50f + temp * 0.1f, 1.0f);
}

MobList& Biome::getMobs(EntityType category){
	if (EntityClassTree::isTypeInstanceOf(category, EntityType::Monster)) {
		return _enemies;
	}
	if (EntityClassTree::isTypeInstanceOf(category, EntityType::Animal)) {
		return _friendlies;
	}
	if (EntityClassTree::isTypeInstanceOf(category, EntityType::WaterAnimal)) {
		return _waterFriendlies;
	}

	DEBUG_FAIL("Unknown category");
	return _emptyMobList;
}

float Biome::getCreatureProbability() {
	return 0.08f;
}

int Biome::getFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getFoliageColor(temp, rain);
}

int Biome::getBirchFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getBirchColor(temp, rain);
}

int Biome::getEvergreenFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getEvergreenColor(temp, rain);
}

int Biome::getMapFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getMapFoliageColor(temp, rain);
}

int Biome::getMapBirchFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getMapBirchColor(temp, rain);
}

int Biome::getMapEvergreenFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getMapEvergreenColor(temp, rain);
}

float Biome::getDownfall() const {
	return mDownfall;
}

void Biome::decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float worldLimit){
	decorator->decorate(source, random, this, origin, legacy, worldLimit);
}

FullBlock Biome::getRandomFlowerTypeAndData(Random& random, const BlockPos& pos){
	return Block::mYellowFlower->mID;
}

void Biome::buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) {
	buildSurfaceAtDefault(random, blocks, pos, depthValue, seaLevel);
}

void Biome::_placeBedrock(Random& random, BlockVolume& blocks, const BlockPos& pos) {
	auto chunkPos = (ChunkBlockPos)pos;
	auto p = Pos{ chunkPos.x, chunkPos.y, chunkPos.z };

	for (auto h : range_incl(random.nextInt(1, 5))) {
		p.y = h;
		blocks.setBlock(p, Block::mBedrock->mID);
	}
}

void Biome::buildSurfaceAtDefault(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) {

	FullBlock top(mTopMaterial, mTopMaterialData);
	BlockID material = mMaterial;
	int run = -1;
	int runDepth = (int)(depthValue / 3 + 3 + random.nextFloat() * 0.25f);

	auto p = (ChunkBlockPos)pos;

	_placeBedrock(random, blocks, pos);

	for (p.y = blocks.getHeight() - 1; p.y > 0; p.y--) {
		auto old = blocks.block(p);

		if (old == Block::mBedrock->mID) {
			break;
		}

		if(old == 0) {
			run = -1;
		}
		else if(old == Block::mStone->mID) {
			if(run == -1) {
				if(runDepth <= 0) {
					top = BlockID::AIR;
					material = Block::mStone->mID;
				}
				else if(p.y >= seaLevel - 4 && p.y <= seaLevel + 1) {
					top = FullBlock(mTopMaterial, mTopMaterialData);
					material = mMaterial;
				}
				if(p.y < seaLevel && top.isAir()) {
					if(getTemperature() < 0.15f) {
						top = Block::mIce->mID;
					}
					else {
						top = Block::mStillWater->mID;
					}
				}

				run = runDepth;
				if(p.y >= seaLevel - 1) {
					blocks.setBlock(p, top.id);
					blocks.setData(p, top.data);
				}
				else if(p.y < seaLevel - 7 - runDepth) {
					top.id = BlockID::AIR;
					material = Block::mStone->mID;
					blocks.setBlock(p, Block::mGravel->mID);
				}
				else {
					blocks.setBlock(p, material);
				}
			}
			else if(run > 0) {
				run--;
				blocks.setBlock(p, material);

				// place a few sandstone blocks beneath sand runs
				if(run == 0 && material == Block::mSand->mID) {
					run = random.nextInt(4) + std::max(0, p.y - 63);
					material = Block::mSandStone->mID;
				}
			}
		}
	}
}

int Biome::getGrassColor( const BlockPos& pos ) {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getGrassColor(temp, rain);
}

int Biome::getMapGrassColor(const BlockPos& pos) {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getMapGrassColor(temp, rain);
}

void Biome::refreshBiomes(RandomSeed levelSeed) {
	for(auto a : range(BIOMES_COUNT)) {
		Biome* biome = mBiomes[a];
		if (biome != nullptr) {
			biome->refreshBiome(levelSeed);
		}
	}
}

void Biome::ResetInstanceData() {

	for (auto a : range(BIOMES_COUNT)) {
		Biome* biome = mBiomes[a];
		if (biome == nullptr) {
			continue;
		}

		biome->mSnowAccumulation = 0.0f;
		biome->mFoliageSnow = 0.0f;
	}
}

void Biome::SaveInstanceData(LevelStorage& storage) {

	Unique<ListTag> biomeTags = make_unique<ListTag>();

	for(auto a : range(BIOMES_COUNT)) {
		Biome* biome = mBiomes[a];
		if (biome == nullptr) {
			continue;
		}

		Unique<CompoundTag> ctag = make_unique<CompoundTag>();

		// Save Instance Data
		if (biome->mSnowAccumulation > 0) {
			ctag->putFloat("snowAccumulation", biome->mSnowAccumulation);
		}

		if (biome->mFoliageSnow > 0) {
			ctag->putFloat("foliageSnow", biome->mFoliageSnow);
		}

		if (!ctag->isEmpty()) {
			ctag->putByte("id", biome->mId);
			biomeTags->add(std::move(ctag));
		}
	}

	if (biomeTags->size() > 0) {
		Unique<CompoundTag> ctag = make_unique<CompoundTag>("root");
		ctag->put("list", std::move(biomeTags));
		storage.saveData("BiomeData", *ctag.get());
	}
}

void Biome::LoadInstanceData(LevelStorage& storage) {

	// Ok now lets load our biome instance data
	std::string uncompressedBuf = storage.loadData("BiomeData");
	if (uncompressedBuf.empty()) {
		return;
	}

	// Read data from buffer to tags
	StringByteInput input(uncompressedBuf);
	Unique<CompoundTag> root = NbtIo::read(input);
	if (const ListTag* biomeTags = root->getList("list")) {
		for (int i = 0; i < biomeTags->size(); ++i) {
			Tag* t = biomeTags->get(i);
			if (t->getId() == Tag::Type::Compound) {
				CompoundTag* ctag = static_cast<CompoundTag*>(t);
				unsigned char id = static_cast<unsigned char>(ctag->getByte("id"));

				Biome* biome = mBiomes[id];
				if (biome == nullptr) {
					continue;
				}

				// Sets to zero if tag is missing (which is what we want)
				biome->mSnowAccumulation = ctag->getFloat("snowAccumulation");
				biome->mFoliageSnow = ctag->getFloat("foliageSnow");
			}
		}
	}
}

Unique<Biome> Biome::createMutatedCopy() {
	return createMutatedCopy(mId + MUTATED_OFFSET);
}

Unique<Biome> Biome::createMutatedCopy(int id) {
	return make_unique<MutatedBiome>(id, this);
}

Biome::BiomeType Biome::getBiomeType() {
	return mBiomeType;
}

Biome::BiomeTempCategory Biome::getTemperatureCategory() const {
	if(mTemperature < 0.2f) {
		return BiomeTempCategory::COLD;
	}
	if(mTemperature < 1.0f) {
		return BiomeTempCategory::MEDIUM;
	}
	return BiomeTempCategory::WARM;
}

bool Biome::isSame(Biome* other) {
	if(other == this) {
		return true;
	}
	if(other == nullptr) {
		return false;
	}
	return getBiomeType() == other->getBiomeType();
}

int Biome::getDownfallInt() {
	return int(mDownfall * 65536);
}

bool Biome::isSnowCovered() {
	return mSnowAccumulation > 0.0f;
}

bool Biome::isHumid(){
	return mDownfall > .85f;
}

float Biome::getTemperature() {
	// It does not get colder higher up in Pocket Edition
//  if(pos.y > 64) {
//      float v = (float)(temperatureNoise->getValue(pos.x * 1.0f / 8, pos.z * 1.0f / 8) * 4.0f);
//      return temperature - (v + pos.y - 64) * .05f / 30.0f;
//  }
	return mTemperature;
}
