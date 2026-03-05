#pragma once
#include "Feature.h"

class SpikeFeature : public Feature
{
private:
	int tile;
	//int m_iIndex;

public:
	SpikeFeature(int tile);
	virtual bool place(Level *level, Random *random, int x, int y, int z);
	virtual bool placeWithIndex(Level *level, Random *random, int x, int y, int z,int iIndex, int iRadius);
};
