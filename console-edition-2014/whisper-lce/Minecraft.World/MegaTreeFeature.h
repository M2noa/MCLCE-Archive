#pragma once

#include "Feature.h"

class MegaTreeFeature : public Feature
{
private:
	const int baseHeight;
	const int trunkType;
	const int leafType;

public:
	MegaTreeFeature(bool doUpdate, int baseHeight, int trunkType, int leafType);

	bool place(Level *level, Random *random, int x, int y, int z);

private:
	void placeLeaves(Level *level, int x, int z, int topPosition, int baseRadius, Random *random);
};