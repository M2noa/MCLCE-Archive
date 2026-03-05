#pragma once
#include "Tile_SPU.h"

class ClothTile_SPU : public Tile_SPU
{
public:
	ClothTile_SPU(int id) : Tile_SPU(id) {}
	virtual Icon_SPU *getTexture(int face, int data) {	return &ms_pTileData->clothTile_icons[data]; }


};