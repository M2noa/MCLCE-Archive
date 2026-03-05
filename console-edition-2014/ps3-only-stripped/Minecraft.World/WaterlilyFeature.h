#pragma once
#include "Feature.h"

class WaterlilyFeature : public Feature
{
	virtual bool place(Level *level, Random *random, int x, int y, int z);
};