#pragma once

#include "EntityTile_SPU.h"

class EnderChestTile_SPU : public EntityTile_SPU
{
public:
	EnderChestTile_SPU(int id) : EntityTile_SPU(id) {}

	bool isSolidRender(bool isServerLevel = false) { return false; }
	bool isCubeShaped()  { return false; }
	int getRenderShape() { 	return Tile_SPU::SHAPE_ENTITYTILE_ANIMATED; }
};