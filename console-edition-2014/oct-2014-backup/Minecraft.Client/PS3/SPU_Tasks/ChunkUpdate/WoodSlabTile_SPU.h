#pragma once

#include "HalfSlabTile_SPU.h"

class WoodSlabTile_SPU : HalfSlabTile_SPU
{	
public:
	static const int TYPE_MASK = 7;

	WoodSlabTile_SPU(int id) : HalfSlabTile_SPU(id) {}
	virtual Icon_SPU *getTexture(int face, int data) 
	{ 
		return TileRef_SPU(wood_Id)->getTexture(face, data & TYPE_MASK);
	}
};