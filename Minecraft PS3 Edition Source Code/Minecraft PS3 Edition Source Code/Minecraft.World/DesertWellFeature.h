#pragma once

#include "Feature.h"

class DesertWellFeature : public Feature
{
public:
	bool place(Level *level, Random *random, int x, int y, int z);
};