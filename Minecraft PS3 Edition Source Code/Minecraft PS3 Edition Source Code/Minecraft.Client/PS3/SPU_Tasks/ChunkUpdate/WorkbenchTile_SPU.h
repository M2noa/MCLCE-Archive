#pragma once

#include "Tile_SPU.h"

class Player;

class WorkbenchTile_SPU : public Tile_SPU
{
public:
	WorkbenchTile_SPU(int id) : Tile_SPU(id) {}
	Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP) return &ms_pTileData->workBench_iconTop;
		if (face == Facing::DOWN) return TileRef_SPU(wood_Id)->getTexture(face);
		if (face == Facing::NORTH || face == Facing::WEST) return &ms_pTileData->workBench_iconFront;
		return icon();
	}
};