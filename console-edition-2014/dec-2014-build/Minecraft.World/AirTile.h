#pragma once
#include "Tile.h"
#include "Material.h"

class AirTile : public Tile
{
	friend class Tile;

protected:
	AirTile(int id);
};