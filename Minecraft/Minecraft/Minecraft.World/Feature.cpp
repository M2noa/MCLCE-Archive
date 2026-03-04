#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "Feature.h"


Feature::Feature()
{
	this->doUpdate = false;
}

Feature::Feature(bool doUpdate)
{
	this->doUpdate = doUpdate;
}

void Feature::placeBlock(Level *level, int x, int y, int z, int tile)
{
	placeBlock(level, x, y, z, tile, 0);
}

void Feature::placeBlock(Level *level, int x, int y, int z, int tile, int data)
{
	if (doUpdate)
	{
		level->setTileAndData(x, y, z, tile, data);
	}
	else
	{
		level->setTileAndDataNoUpdate(x, y, z, tile, data);
	}
}
