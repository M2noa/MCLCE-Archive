#pragma once
#include "Bush_SPU.h"


class CropTile_SPU : public Bush_SPU
{
public:
	CropTile_SPU(int id) : Bush_SPU(id) {}
	virtual Icon_SPU *getTexture(int face, int data)
	{
		if (data < 0 || data > 7) data = 7;
		return &ms_pTileData->cropTile_icons[data];
	}

    virtual int getRenderShape()
	{
		return Tile_SPU::SHAPE_ROWS;
	}
};