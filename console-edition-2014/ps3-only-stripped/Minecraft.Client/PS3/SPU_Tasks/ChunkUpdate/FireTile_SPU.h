#pragma once
#include "Tile_SPU.h"


class FireTile_SPU : public Tile_SPU
{
public:
	FireTile_SPU(int id) : Tile_SPU(id) {}

	virtual bool blocksLight() { return false; }
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
    virtual bool isCubeShaped() { return false; }
    virtual int getRenderShape() {return Tile_SPU::SHAPE_FIRE; }

	Icon_SPU *getTextureLayer(int layer)  { return &ms_pTileData->fireTile_icons[layer];}
	Icon_SPU *getTexture(int face, int data) { return &ms_pTileData->fireTile_icons[0];}

	static bool canBurn(ChunkRebuildData *level, int x, int y, int z)
	{
		int id = level->getTile(x, y, z);
		switch (id)
		{
		case Tile_SPU::wood_Id:	
		case Tile_SPU::woodSlab_Id:
		case Tile_SPU::woodSlabHalf_Id:
		case Tile_SPU::fence_Id:
		case Tile_SPU::stairs_wood_Id:
		case Tile_SPU::stairs_birchwood_Id:
		case Tile_SPU::stairs_sprucewood_Id:
		case Tile_SPU::stairs_junglewood_Id:
		case Tile_SPU::treeTrunk_Id:
		case Tile_SPU::leaves_Id:
		case Tile_SPU::bookshelf_Id:
		case Tile_SPU::tnt_Id:
		case Tile_SPU::tallgrass_Id:
		case Tile_SPU::cloth_Id:
		case Tile_SPU::vine_Id:
			return true;
		default:
			return false;
		}
		return false;
	}
};