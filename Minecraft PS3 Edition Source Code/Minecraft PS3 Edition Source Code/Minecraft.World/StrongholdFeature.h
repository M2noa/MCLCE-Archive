#pragma once

class Biome;
#include "StructureFeature.h"

#include "StructureStart.h"


#include "ChunkPos.h"

// 4J Stu Added
// We can get away with a few more attempts on new-gen consoles
#ifdef _LARGE_WORLDS
#define MAX_STRONGHOLD_ATTEMPTS 30
#else
#define MAX_STRONGHOLD_ATTEMPTS 10
#endif

class StrongholdFeature : public StructureFeature
{
public:
	static void staticCtor();
private:
	static vector<Biome *> allowedBiomes;

    bool isSpotSelected;
	static const int strongholdPos_length = 1;// Java game has 3, but xbox game only has 1 because of the world size;	// 4J added
    ChunkPos *strongholdPos[strongholdPos_length];

public:
	StrongholdFeature();
	~StrongholdFeature();

protected:
	virtual bool isFeatureChunk(int x, int z, bool bIsSuperflat=false);
	vector<TilePos> *getGuesstimatedFeaturePositions();
    virtual StructureStart *createStructureStart(int x, int z);

private:
	class StrongholdStart : public StructureStart
	{
	public:
		StrongholdStart(Level *level, Random *random, int chunkX, int chunkZ);
    };
};
