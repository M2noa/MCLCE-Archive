#pragma once
#include "EntityTile_SPU.h"

class EnchantmentTableTile_SPU : public EntityTile_SPU
{
public:
	EnchantmentTableTile_SPU(int id) : EntityTile_SPU(id) {}

	bool isCubeShaped() { return false; }
    bool isSolidRender(bool isServerLevel = false) { return false; }
    Icon_SPU *getTexture(int face, int data)
	{
		if (face == Facing::DOWN) return &ms_pTileData->enchantmentTable_iconBottom;
		if (face == Facing::UP) return &ms_pTileData->enchantmentTable_iconTop;
		return icon();
	}
};