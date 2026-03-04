#pragma once
#include "Tile.h"

class Random;

class LockedChestTile : public Tile
{
	friend class Tile;
protected:
	LockedChestTile(int id);
    virtual bool mayPlace(Level *level, int x, int y, int z);
    virtual void tick(Level *level, int x, int y, int z, Random *random);
	void registerIcons(IconRegister *iconRegister);
};