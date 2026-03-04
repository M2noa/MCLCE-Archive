#pragma once
#include "Tile_SPU.h"

class MycelTile_SPU : public Tile_SPU
{
public:
	MycelTile_SPU(int id) : Tile_SPU(id) {}

	virtual Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP) return &ms_pTileData->mycelTile_iconTop;
		if (face == Facing::DOWN) return TileRef_SPU(dirt_Id)->getTexture(face);
		return icon();
	}
    virtual Icon_SPU *getTexture(ChunkRebuildData *level, int x, int y, int z, int face)
	{
		if (face == Facing::UP) return &ms_pTileData->mycelTile_iconTop;
		if (face == Facing::DOWN) return TileRef_SPU(dirt_Id)->getTexture(face);
		Material_SPU *above = level->getMaterial(x, y + 1, z);
		if (above->getID() == Material_SPU::topSnow_Id || above->getID() == Material_SPU::snow_Id) 
			return &ms_pTileData->mycelTile_iconSnowSide;
		else return icon();

	}
};
