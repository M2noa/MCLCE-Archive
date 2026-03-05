#pragma once

#include "Tile.h"

class Sponge : public Tile
{
	friend class Tile;
public:
	static const int RANGE = 2;

protected:
	Sponge(int id);
};