#pragma once
#include "EntityTile_SPU.h"

class ChestTile_SPU : public EntityTile_SPU
{
public:
	ChestTile_SPU(int id) : EntityTile_SPU(id) {}
	virtual bool isSolidRender(bool isServerLevel = false) { return false; }
	virtual int getRenderShape() { 	return Tile_SPU::SHAPE_ENTITYTILE_ANIMATED; }
};