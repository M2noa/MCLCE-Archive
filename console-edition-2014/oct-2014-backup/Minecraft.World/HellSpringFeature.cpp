#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "HellSpringFeature.h"
#include "net.minecraft.world.level.tile.h"

HellSpringFeature::HellSpringFeature(int tile)
{
	this->tile = tile;
}

bool HellSpringFeature::place(Level *level, Random *random, int x, int y, int z)
{
    if (level->getTile(x, y + 1, z) != Tile::hellRock_Id) return false;
    if (level->getTile(x, y - 1, z) != Tile::hellRock_Id) return false;

    if (level->getTile(x, y, z) != 0 && level->getTile(x, y, z) != Tile::hellRock_Id) return false;

    int rockCount = 0;
    if (level->getTile(x - 1, y, z) == Tile::hellRock_Id) rockCount++;
    if (level->getTile(x + 1, y, z) == Tile::hellRock_Id) rockCount++;
    if (level->getTile(x, y, z - 1) == Tile::hellRock_Id) rockCount++;
    if (level->getTile(x, y, z + 1) == Tile::hellRock_Id) rockCount++;
	if (level->getTile(x, y - 1, z) == Tile::hellRock_Id) rockCount++;

    int holeCount = 0;
    if (level->isEmptyTile(x - 1, y, z)) holeCount++;
    if (level->isEmptyTile(x + 1, y, z)) holeCount++;
    if (level->isEmptyTile(x, y, z - 1)) holeCount++;
    if (level->isEmptyTile(x, y, z + 1)) holeCount++;
	if (level->isEmptyTile(x, y - 1, z)) holeCount++;

    if (rockCount == 4 && holeCount == 1)
	{
        level->setTile(x, y, z, tile);
        level->setInstaTick(true);
        Tile::tiles[tile]->tick(level, x, y, z, random);
        level->setInstaTick(false);
    }

    return true;

}