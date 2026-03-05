#pragma once
#include "EntityTile_SPU.h"


class DispenserTile_SPU : public EntityTile_SPU
{

public:
	static const int FACING_MASK = 0x7;
	DispenserTile_SPU(int id) : EntityTile_SPU(id) {}

public:
	virtual Icon_SPU *getTexture(int face, int data)
	{
		int dir = data & FACING_MASK;
		if (face == dir)
		{
			if (dir == Facing::UP || dir == Facing::DOWN)
			{
				return &ms_pTileData->dispenserTile_iconFrontVertical;
			}
			else
			{
				return &ms_pTileData->dispenserTile_iconFront;
			}
		}

		if (dir == Facing::UP || dir == Facing::DOWN)
		{
			return &ms_pTileData->dispenserTile_iconTop;
		}
		else if (face == Facing::UP || face == Facing::DOWN)
		{
			return &ms_pTileData->dispenserTile_iconTop;
		}
		return icon();
	}
};