#pragma once
#include "Tile_SPU.h"

class PumpkinTile_SPU : public Tile_SPU
{
public:
	static const int DIR_SOUTH = 0;
	static const int DIR_WEST = 1;
	static const int DIR_NORTH = 2;
	static const int DIR_EAST = 3;

public:
	PumpkinTile_SPU(int id) : Tile_SPU(id) {}
    virtual Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP) return &ms_pTileData->pumpkinTile_iconTop;
		if (face == Facing::DOWN) return &ms_pTileData->pumpkinTile_iconTop;

		Icon_SPU* iconFace = &ms_pTileData->pumpkinTile_iconFace;
		if(id == litPumpkin_Id)
			iconFace = &ms_pTileData->pumpkinTile_iconFaceLit;

		if (data == DIR_NORTH && face == Facing::NORTH) return iconFace;
		if (data == DIR_EAST && face == Facing::EAST) return iconFace;
		if (data == DIR_SOUTH && face == Facing::SOUTH) return iconFace;
		if (data == DIR_WEST && face == Facing::WEST) return iconFace;

		else return icon();
	}
};