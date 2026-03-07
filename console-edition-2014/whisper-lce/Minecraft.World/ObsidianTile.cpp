#include "stdafx.h"
#include "ObsidianTile.h"

ObsidianTile::ObsidianTile(int id) : StoneTile(id)
{
}

int ObsidianTile::getResourceCount(Random *random)
{
	return 1;
}

int ObsidianTile::getResource(int data, Random *random, int playerBonusLevel)
{
	return Tile::obsidian_Id;
}