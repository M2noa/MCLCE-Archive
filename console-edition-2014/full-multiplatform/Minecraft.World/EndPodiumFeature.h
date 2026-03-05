#pragma once
#include "Feature.h"

class EndPodiumFeature : public Feature
{
private:
	int tile;
	//int m_iIndex;

public:
	EndPodiumFeature(int tile);
	virtual bool place(Level *level, Random *random, int x, int y, int z);

};
