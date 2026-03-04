#pragma once

#include "TileItem.h"

class Tile;

class AuxDataTileItem : public TileItem
{
private:
	Tile *parentTile;

public:
	AuxDataTileItem(int id, Tile *parentTile);

	Icon *getIcon(int itemAuxValue);
	int getLevelDataForAuxValue(int auxValue);
};