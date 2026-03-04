#pragma once
#include "Bush_SPU.h"

class NetherStalkTile_SPU : public Bush_SPU
{
private:
	static const int MAX_AGE = 3;
	static const int NETHER_STALK_TEXTURE_COUNT = 3;


public:
	NetherStalkTile_SPU(int id) : Bush_SPU(id) {}

	virtual Icon_SPU *getTexture(int face, int data)
	{
		Icon_SPU* icons = ms_pTileData->netherStalk_icons;
		if (data >= MAX_AGE)
		{
			return &icons[2];
		}
		if (data > 0)
		{
			return &icons[1];
		}
		return &icons[0];

	}
	virtual int getRenderShape() { 	return Tile_SPU::SHAPE_ROWS; }
};
