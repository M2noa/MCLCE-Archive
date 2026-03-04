#pragma once
#include "RailTile_SPU.h"


class DetectorRailTile_SPU : public RailTile_SPU
{
public:
	DetectorRailTile_SPU(int id) : RailTile_SPU(id) {}
	Icon_SPU *getTexture(int face, int data)
	{
		if ((data & RAIL_DATA_BIT) != 0)
		{
			return &ms_pTileData->detectorRailTile_icons[1];
		}
		return &ms_pTileData->detectorRailTile_icons[0];
	}
};
