#pragma once

#include "Tile.h"

class SpringTile : public Tile
{
private:
	int liquidTileId;

protected:
	SpringTile(int id, int liquidTileId);

public:
	void onPlace(Level *level, int x, int y, int z);

	void tick(Level *level, int x, int y, int z, Random *random);
};