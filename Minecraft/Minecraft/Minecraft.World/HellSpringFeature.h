#pragma once
#include "Feature.h"


class HellSpringFeature : public Feature
{
private:
	int tile;

public:
	HellSpringFeature(int tile);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};