#pragma once
#include "Bush_SPU.h"

class WaterlilyTile_SPU : public Bush_SPU
{
private:
	static const int col = 0x208030;

public:
	WaterlilyTile_SPU(int id) : Bush_SPU(id) {}

	virtual int getRenderShape() { 	 return Tile_SPU::SHAPE_LILYPAD; }
	virtual int getColor(LevelSource *level, int x, int y, int z) { return ms_pTileData->waterLily_colour;/*return col;*/ }
	virtual int getColor() { return ms_pTileData->waterLily_colour; /*return col;*/ }
};
