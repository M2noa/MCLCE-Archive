#pragma once

#include "CropTile_SPU.h"

class CarrotTile_SPU : public CropTile_SPU
{
private:
// 	Icon *icons[4];
public:
	CarrotTile_SPU(int id) : CropTile_SPU(id) {}

	Icon_SPU *getTexture(int face, int data)
	{
		if (data < 7)
		{
			if (data == 6)
			{
				data = 5;
			}
			return &ms_pTileData->carrot_icons[data >> 1];
		}
		else
		{
			return &ms_pTileData->carrot_icons[3];
		}
	}

};