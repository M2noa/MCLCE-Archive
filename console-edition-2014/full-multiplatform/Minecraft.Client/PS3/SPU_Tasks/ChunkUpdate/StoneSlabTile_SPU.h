#pragma once

#include "HalfSlabTile_SPU.h"
#include "Facing_SPU.h"

class StoneSlabTile_SPU : public HalfSlabTile_SPU
{
public:
	static const int STONE_SLAB = 0;
	static const int SAND_SLAB = 1;
	static const int WOOD_SLAB = 2;
	static const int COBBLESTONE_SLAB = 3;
	static const int BRICK_SLAB = 4;
	static const int SMOOTHBRICK_SLAB = 5;
	static const int NETHERBRICK_SLAB = 6;
	static const int QUARTZ_SLAB = 7;

	static const int SLAB_NAMES_LENGTH = 8;



public:
	StoneSlabTile_SPU(int id) : HalfSlabTile_SPU(id) {}

	virtual Icon_SPU *getTexture(int face, int data)
	{
		int type = data & TYPE_MASK;
		if (fullSize() && (data & TOP_SLOT_BIT) != 0)
		{
			face = Facing::UP;
		}
		switch(type)
		{
		case STONE_SLAB:
			if (face == Facing::UP || face == Facing::DOWN) 
				return icon();
			return &ms_pTileData->stoneSlab_iconSide;
			break;	
		case SAND_SLAB:
			return TileRef_SPU(sandStone_Id)->getTexture(face); //Tile::sandStone->getTexture(face);
		case WOOD_SLAB:
			return TileRef_SPU(wood_Id)->getTexture(face); //Tile::wood->getTexture(face);
		case COBBLESTONE_SLAB:
			return TileRef_SPU(stoneBrick_Id)->getTexture(face); //Tile::stoneBrick->getTexture(face);
		case BRICK_SLAB:
			return TileRef_SPU(redBrick_Id)->getTexture(face); //Tile::redBrick->getTexture(face);
 		case SMOOTHBRICK_SLAB:
 			return TileRef_SPU(stoneBrickSmooth_Id)->getTexture(face); //Tile::stoneBrickSmooth->getTexture(face, SmoothStoneBrickTile::TYPE_DEFAULT);
 		case NETHERBRICK_SLAB:
 			return TileRef_SPU(netherBrick_Id)->getTexture(Facing::UP); //Tile::netherBrick->getTexture(Facing::UP);
		case QUARTZ_SLAB:
			return TileRef_SPU(quartzBlock_Id)->getTexture(face); //Tile::quartzBlock->getTexture(face);

		}

		return icon();
	}


};