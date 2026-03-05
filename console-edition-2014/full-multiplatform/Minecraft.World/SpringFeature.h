#pragma once
#include "Feature.h"


class SpringFeature : public Feature
{
private:
	int tile;

public:
	SpringFeature(int tile);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};