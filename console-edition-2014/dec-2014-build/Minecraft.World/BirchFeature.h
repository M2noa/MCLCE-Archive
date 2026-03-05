#pragma once
#include "Feature.h"

class Level;

class BirchFeature : public Feature
{
public:
	BirchFeature(bool doUpdate);
    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
