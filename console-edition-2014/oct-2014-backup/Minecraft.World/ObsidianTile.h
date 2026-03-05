#pragma once
#include "StoneTile.h"

class Random;

class ObsidianTile : public StoneTile
{
public:
	ObsidianTile(int id);
    virtual int getResourceCount(Random *random);
    virtual int getResource(int data, Random *random, int playerBonusLevel);
};