#pragma once
#include "Tile.h"

class DirtTile : public Tile
{
	friend class Tile;
protected:
	DirtTile(int id);
};