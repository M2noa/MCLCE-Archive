#pragma once

#include "Feature.h"

class VinesFeature : public Feature
{

public:
	VinesFeature();

	bool place(Level *level, Random *random, int x, int y, int z);
};