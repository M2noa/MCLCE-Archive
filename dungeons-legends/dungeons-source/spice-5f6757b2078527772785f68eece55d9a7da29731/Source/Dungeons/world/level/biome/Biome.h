#pragma once

#include "common_header.h"

#include "CommonTypes.h"

#include "world/level/biome/MobSpawnerData.h"
#include "Core/Math/Color.h"
#include "BiomeDecorator.h"
#include "Core/Utility/buffer_span.h"
#include "util/NibblePair.h"

class Feature;
class BiomeDecorator;
class BlockSource;
class Random;
class BlockPos;
class PerlinSimplexNoise;
class LevelStorage;
class BlockVolume;

typedef std::vector<MobSpawnerData> MobList;

class Biome;
typedef std::unique_ptr<Biome> BiomePtr;

class Biome {
public:

	static const Color DEFAULT_WATER_COLOR, DEFAULT_UNDERWATER_COLOR;

	static const int MUTATED_OFFSET = 128;
	static const int BIOMES_COUNT = 256;
	static const float RAIN_TEMP_THRESHOLD;

protected:

	struct BiomeHeight {
		float depth;
		float scale;
		static const float MIN_DEPTH;
		BiomeHeight(float depth, float scale) :
			depth(depth)
			, scale(scale) {

		}

		BiomeHeight less() const {
			return BiomeHeight(depth * 0.8f, scale * 0.8f);
		}

	};

public:

	enum class BiomeType {
		Beach,
		Desert,
		ExtremeHills,
		Flat,
		Forest,
		Hell,
		Ice,
		Jungle,
		Mesa,
		MushroomIsland,
		Ocean,
		Plain,
		River,
		Savanna,
		StoneBeach,
		Swamp,
		Taiga,
		TheEnd
	};

	enum class BiomeTempCategory {
		OCEAN,
		COLD,
		MEDIUM,
		WARM
	};

	static const BiomeHeight HEIGHTS_DEFAULT;
	static const BiomeHeight HEIGHTS_RIVER;
	static const BiomeHeight HEIGHTS_OCEAN;
	static const BiomeHeight HEIGHTS_DEEP_OCEAN;
	static const BiomeHeight HEIGHTS_LOWLANDS;
	static const BiomeHeight HEIGHTS_TAIGA;
	static const BiomeHeight HEIGHTS_MOUNTAINS;
	static const BiomeHeight HEIGHTS_HIGHLANDS;
	static const BiomeHeight HEIGHTS_EXTREME;
	static const BiomeHeight HEIGHTS_BEACH;
	static const BiomeHeight HEIGHTS_STONE_BEACH;	// check
	static const BiomeHeight HEIGHTS_MUSHROOM;
	static const BiomeHeight HEIGHTS_SWAMPLAND;

	static BiomePtr ocean;
	static BiomePtr plains;
	static BiomePtr desert;

	static BiomePtr extremeHills;
	static BiomePtr forest;
	static BiomePtr taiga;

	static BiomePtr swampland;
	static BiomePtr river;

	static BiomePtr hell;
	static BiomePtr sky;

	static BiomePtr frozenOcean;
	static BiomePtr frozenRiver;
	static BiomePtr iceFlats;
	static BiomePtr iceMountains;

	static BiomePtr mushroomIsland;
	static BiomePtr mushroomIslandShore;

	static BiomePtr beaches;
	static BiomePtr desertHills;
	static BiomePtr forestHills;
	static BiomePtr taigaHills;
	static BiomePtr smallerExtremeHills;

	static BiomePtr jungle;
	static BiomePtr jungleHills;
	static BiomePtr jungleEdge;

	static BiomePtr deepOcean;
	static BiomePtr stoneBeach;
	static BiomePtr coldBeach;

	static BiomePtr birchForest;
	static BiomePtr birchForestHills;
	static BiomePtr roofedForest;

	static BiomePtr taigaCold;
	static BiomePtr taigaColdHills;
	static BiomePtr redwoodTaiga;
	static BiomePtr redwoodTaigaHills;

	static BiomePtr extremeHillsWithTrees;

	static BiomePtr savanna;
	static BiomePtr savannaRock;

	static BiomePtr mesa;
	static BiomePtr mesaRock;
	static BiomePtr mesaClearRock;

	static BiomePtr plainsMutated;
	static BiomePtr desertMutated;
	static BiomePtr forestMutated;
	static BiomePtr taigaMutated;
	static BiomePtr swamplandMutated;
	static BiomePtr iceFlatsMutated;
	static BiomePtr jungleMutated;
	static BiomePtr jungleEdgeMutated;
	static BiomePtr taigaColdMutated;
	static BiomePtr savannaMutated;
	static BiomePtr savannaRockMutated;
	static BiomePtr mesaMutated;
	static BiomePtr mesaRockMutated;
	static BiomePtr mesaClearRockMutated;
	static BiomePtr birchForestMutated;
	static BiomePtr birchForestHillsMutated;
	static BiomePtr roofedForestMutated;
	static BiomePtr redwoodTaigaMutated;
	static BiomePtr extremeHillsMutated;
	static BiomePtr extremeHillsWithTreesMutated;
	static BiomePtr redwoodTaigaHillsMutated;
	static Biome* DEFAULT;

public:

	static const MobList EMPTY_MOBLIST;

protected:

	Biome(int id, BiomeType biomeType, Unique<BiomeDecorator> customDecorator = nullptr);
	
	virtual Biome& setColor(int color);
	virtual Biome& setColor(int color, bool oddColor);

	static MobList _emptyMobList;

public:

	const Unique<BiomeDecorator> decorator;

	Biome& setOddColor(int color);
	Biome& setTemperatureAndDownfall(float temperature, float downfall);
	Biome& clearMobs(bool friendlies = true, bool waterFriendlies = true, bool enemies = true);
	Biome& setDepthAndScale(const BiomeHeight& heightData);
	Biome& setNoRain();
	Biome& setSnowCovered();	// Shorthand for setSnowAccumulation(1.0f/8.0f,1.0f);
	Biome& setSnowAccumulation(float minSnowLevel = 0.0f, float maxSnowLevel = 1.0f);	// 1.0f is 1 block (or 8
																						// layers), and 0.125 is 1 out
																						// of 8 snow layers.
	int getSnowAccumulationLayers() const;

	int getMinSnowLayers() const {
		return static_cast<int>(mMinSnowLevel * 8.0f);
	}

	int getMaxSnowLayers() const {
		return static_cast<int>(mMaxSnowLevel * 8.0f);
	}

	bool canHaveSnowfall() const {
		return mMaxSnowLevel > 0.0f && mTemperature < RAIN_TEMP_THRESHOLD;
	}

	Biome& setName(const std::string& name);
	Biome& setLeafColor(int leafColor);

	MobList _enemies;
	MobList _friendlies;
	MobList _waterFriendlies;
	MobList _ambientFriendlies;
	static int defaultTotalEnemyWeight;
	static int defaultTotalFriendlyWeight;
	static void initBiomes();

	virtual ~Biome();

	virtual const FeaturePtr& getTreeFeature(Random* random);
	virtual const FeaturePtr& getGrassFeature(Random* random);

	virtual float getTemperature();

	int getDebugMapColor(int x, int y);

	static Biome* getBiome(int id);
	static Biome* getBiome(int id, Biome* defaultBiome);

	virtual float adjustScale(float scale);
	virtual float adjustDepth(float depth);

	virtual Color getSkyColor(float temp);

	virtual MobList& getMobs(EntityType category);
	virtual float getCreatureProbability();
	
	virtual int getFoliageColor();
	virtual int getBirchFoliageColor();
	virtual int getEvergreenFoliageColor();
	virtual int getMapFoliageColor();
	virtual int getMapBirchFoliageColor();
	virtual int getMapEvergreenFoliageColor();

	float getDownfall() const;

	virtual FullBlock getRandomFlowerTypeAndData(Random& random, const BlockPos& pos);
	virtual void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float worldLimit);
	virtual void buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel);
	void buildSurfaceAtDefault(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel);
	virtual int getGrassColor(const BlockPos& pos);
	virtual int getMapGrassColor(const BlockPos& pos);
	static void refreshBiomes(RandomSeed levelSeed);

	virtual void refreshBiome(RandomSeed levelSeed) {
		UNUSED_PARAMETER(levelSeed);
	}

	BiomeType getBiomeType();
	virtual BiomeTempCategory getTemperatureCategory() const;
	virtual bool isSame(Biome* other);
	int getDownfallInt();
	bool isSnowCovered();

	virtual bool isHumid();

	bool operator==(const BiomePtr& b) const {
		return b->mId == mId;
	}

	bool operator!=(const BiomePtr& b) const {
		return b->mId != mId;
	}

	static void ResetInstanceData();
	static void SaveInstanceData(LevelStorage& levelData);
	static void LoadInstanceData(LevelStorage& levelData);

protected:

	Unique<Biome> createMutatedCopy();

	virtual Unique<Biome> createMutatedCopy(int id);

	void _placeBedrock(Random& random, BlockVolume& blocks, const BlockPos& pos);

public:

	std::string mName;
	int mDebugMapColor, mDebugMapOddColor;
	BlockID mTopMaterial;
	DataID mTopMaterialData;
	BlockID mMaterial;
	int mLeafColor;

	float mTemperature = 0.5;
	float mDownfall = 0.5;
	float mSnowAccumulation = 0.0f;	// 1.0f = 1 full block high of snow
	float mFoliageSnow = 0.0f;
	float mMinSnowLevel = 0.0f, mMaxSnowLevel = 0.0f;

	float mDepth;
	float mScale;
	Color mWaterColor = DEFAULT_WATER_COLOR, mUnderWaterColor = DEFAULT_UNDERWATER_COLOR;
	bool mRain = true;
	BiomeType mBiomeType;
	int mId;
protected:

	static Biome* mBiomes[BIOMES_COUNT];
	Unique<PerlinSimplexNoise> mBiomeInfoNoise;

};
