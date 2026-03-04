#pragma once
#include "Tile_SPU.h"

class Random;

class BookshelfTile_SPU : public Tile_SPU
{
public:
    BookshelfTile_SPU(int id) : Tile_SPU(id) {}

    virtual Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP || face == Facing::DOWN) 
			return TileRef_SPU(wood_Id)->getTexture(face);
		return Tile_SPU::getTexture(face, data);
	}
};