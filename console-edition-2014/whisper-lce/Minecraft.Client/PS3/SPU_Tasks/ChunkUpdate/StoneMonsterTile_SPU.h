#pragma once
#include "Tile_SPU.h"

class Random;

class StoneMonsterTile_SPU : public Tile_SPU
{
public:
	static const int HOST_ROCK = 0;
    static const int HOST_COBBLE = 1;
    static const int HOST_STONEBRICK = 2;

	static const int STONE_MONSTER_NAMES_LENGTH = 3;

	// 4J Stu - I don't know why this is protected in Java
//protected:
public:
    StoneMonsterTile_SPU(int id) : Tile_SPU(id) {}
public:
    virtual Icon_SPU *getTexture(int face, int data)
	{
		if (data == HOST_COBBLE)
		{
			return TileRef_SPU(stoneBrick_Id)->getTexture(face);
		}
		if (data == HOST_STONEBRICK)
		{
			return TileRef_SPU(stoneBrickSmooth_Id)->getTexture(face);
		}
		return TileRef_SPU(rock_Id)->getTexture(face);
	}
};