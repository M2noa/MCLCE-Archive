#pragma once
#include "LiquidTile.h"

class Random;

class LiquidTileStatic : public LiquidTile
{
	friend class Tile;
protected:
	LiquidTileStatic(int id, Material *material);
public:
	virtual bool isPathfindable(LevelSource *level, int x, int y, int z);
	virtual void neighborChanged(Level *level, int x, int y, int z, int type);
private:
	virtual void setDynamic(Level *level, int x, int y, int z);
public:
	virtual void tick(Level *level, int x, int y, int z, Random *random);
private:
	bool isFlammable(Level *level, int x, int y, int z);
};
