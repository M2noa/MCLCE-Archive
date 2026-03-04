#pragma once

#include "CropTile_SPU.h"

class PotatoTile_SPU : public CropTile_SPU
{
private:
//	Icon *icons[4];

public:
	PotatoTile_SPU(int id) : CropTile_SPU(id) {}

	Icon_SPU *getTexture(int face, int data)
	{
		if (data < 7)
		{
			if (data == 6)
			{
				data = 5;
			}
			return &ms_pTileData->potato_icons[data >> 1];
		}
		else
		{
			return &ms_pTileData->potato_icons[3];
		}
	}
};