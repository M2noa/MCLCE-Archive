#pragma once
#include "Feature.h"

class Random;
class Level;

class BonusChestFeature : public Feature
{

private:
	const WeighedTreasureArray treasureList;
    const int numRolls;

public:
	BonusChestFeature(WeighedTreasureArray treasureList, int numRolls);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
	bool place(Level *level, Random *random, int x, int y, int z, bool force);		// 4J added this method with extra force parameter
};
