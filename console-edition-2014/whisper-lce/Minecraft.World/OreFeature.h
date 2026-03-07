#pragma once
#include "Feature.h"

class Level;

class OreFeature : public Feature
{
private:
	int tile;
	int count;
	int targetTile;

	void _init(int tile, int count, int targetTile);
public:
	OreFeature (int tile, int count);
	OreFeature(int tile, int count, int targetTile);

	virtual bool place(Level *level, Random *random, int x, int y, int z);
};