#pragma once
#include "Tile_SPU.h"

class HugeMushroomTile_SPU : public Tile_SPU
{
public:
// 	static const wstring TEXTURE_STEM;
// 	static const wstring TEXTURE_INSIDE;

private:
	static const int HUGE_MUSHROOM_TEXTURE_COUNT = 2;
public:
	HugeMushroomTile_SPU(int id) : Tile_SPU(id) {}
    Icon_SPU *getTexture(int face, int data)
	{
		Icon_SPU* icons = ms_pTileData->hugeMushroom_icons;
		int type = 0; // hugeMushroom1_Id
		if(id == hugeMushroom2_Id)
			type = 1;
		// 123
		// 456 10
		// 789
		if (data == 10 && face > 1) return &ms_pTileData->hugeMushroom_iconStem;
		if (data >= 1 && data <= 9 && face == 1) return &icons[type];
		if (data >= 1 && data <= 3 && face == 2) return &icons[type];
		if (data >= 7 && data <= 9 && face == 3) return &icons[type];

		if ((data == 1 || data == 4 || data == 7) && face == 4) return &icons[type];
		if ((data == 3 || data == 6 || data == 9) && face == 5) return &icons[type];

		// two special cases requested by rhodox (painterly pack)
		if (data == 14)
		{
			return &icons[type];
		}
		if (data == 15)
		{
			return &ms_pTileData->hugeMushroom_iconStem;
		}

		return &ms_pTileData->hugeMushroom_iconInside;
	}
};
