#pragma once
#include "EntityTile_SPU.h"
#include "Facing_SPU.h"

class FurnaceTile_SPU : public EntityTile_SPU
{
public:
	FurnaceTile_SPU(int id) : EntityTile_SPU(id){}
	Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::UP) return &ms_pTileData->furnaceTile_iconTop;
		if (face == Facing::DOWN) return &ms_pTileData->furnaceTile_iconTop;

		if (face != data) return icon();
		if(id == furnace_Id)
			return &ms_pTileData->furnaceTile_iconFront;
		else //furnace_lit_Id
			return &ms_pTileData->furnaceTile_iconFront_lit;
	}
};