#include "stdafx.h"
#include "net.minecraft.world.level.h"

#include "SpringTile.h"


SpringTile::SpringTile(int id, int liquidTileId) : Tile(id, Material::water)
{
	this->liquidTileId = liquidTileId;
	this->setTicking(true);
}

void SpringTile::onPlace(Level *level, int x, int y, int z)
{
	Tile::onPlace(level, x, y, z);
	if (level->isEmptyTile(x-1, y, z)) level->setTile(x-1, y, z, liquidTileId);
	if (level->isEmptyTile(x+1, y, z)) level->setTile(x+1, y, z, liquidTileId);
	if (level->isEmptyTile(x, y, z-1)) level->setTile(x, y, z-1, liquidTileId);
	if (level->isEmptyTile(x, y, z+1)) level->setTile(x, y, z+1, liquidTileId);
}

void SpringTile::tick(Level *level, int x, int y, int z, Random *random)
{
	Tile::tick(level, x, y, z, random);
	if (level->isEmptyTile(x-1, y, z)) level->setTile(x-1, y, z, liquidTileId);
	if (level->isEmptyTile(x+1, y, z)) level->setTile(x+1, y, z, liquidTileId);
	if (level->isEmptyTile(x, y, z-1)) level->setTile(x, y, z-1, liquidTileId);
	if (level->isEmptyTile(x, y, z+1)) level->setTile(x, y, z+1, liquidTileId);
}