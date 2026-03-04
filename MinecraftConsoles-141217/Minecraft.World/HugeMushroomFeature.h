#pragma once
#include "Feature.h"

class HugeMushroomFeature : public Feature
{
private:
	int forcedType;

public:
	HugeMushroomFeature(int forcedType);
	HugeMushroomFeature();
	virtual bool place(Level *level, Random *random, int x, int y, int z);
};
