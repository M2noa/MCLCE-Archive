#pragma once

#include "Tile.h"

class RedlightTile : public Tile
{
private:
	bool isLit;

public:
	RedlightTile(int id, bool isLit);

	virtual void registerIcons(IconRegister *iconRegister);
	virtual void onPlace(Level *level, int x, int y, int z);
	virtual void neighborChanged(Level *level, int x, int y, int z, int type);
	virtual void tick(Level *level, int x, int y, int z, Random *random);
	virtual int getResource(int data, Random *random, int playerBonusLevel);
	virtual int cloneTileId(Level *level, int x, int y, int z);
};