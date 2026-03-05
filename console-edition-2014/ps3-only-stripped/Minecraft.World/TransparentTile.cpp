#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "TransparentTile.h"

TransparentTile::TransparentTile(int id, Material *material, bool allowSame, bool isSolidRender) : Tile(id, material,isSolidRender)
{
	this->allowSame = allowSame;
}

bool TransparentTile::isSolidRender(bool isServerLevel)
{
	return false;
}

bool TransparentTile::shouldRenderFace(LevelSource *level, int x, int y, int z, int face)
{
    int id = level->getTile(x, y, z);
    if (!allowSame && id == this->id) return false;
    return Tile::shouldRenderFace(level, x, y, z, face);
}

bool TransparentTile::blocksLight()
{
	return false;
}