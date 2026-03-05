#pragma once
#include "Tile_SPU.h"
#include "Facing_SPU.h"

class FarmTile_SPU : public Tile_SPU
{
public:
	FarmTile_SPU(int id): Tile_SPU(id) {}
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
    virtual Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP)
		{
			if(data > 0)
				return &ms_pTileData->farmTile_Wet;
			else
				return &ms_pTileData->farmTile_Dry;
		}
		return TileRef_SPU(dirt_Id)->getTexture(face);
	}
    virtual bool blocksLight() { return true; }
};
