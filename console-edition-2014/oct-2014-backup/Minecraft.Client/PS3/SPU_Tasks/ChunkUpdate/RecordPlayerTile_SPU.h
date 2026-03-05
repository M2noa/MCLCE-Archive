#pragma once

#include "EntityTile_SPU.h"


class RecordPlayerTile_SPU : public EntityTile_SPU
{
public:
	RecordPlayerTile_SPU(int id) : EntityTile_SPU(id) {}
	virtual Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP)
		{
			return &ms_pTileData->recordPlayer_iconTop;
		}
		return icon();
	}
};
