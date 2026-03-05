#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "ExtremeHillsBiome.h"

ExtremeHillsBiome::ExtremeHillsBiome(int id) : Biome(id)
{
	friendlies.clear();
}

void ExtremeHillsBiome::decorate(Level *level, Random *random, int xo, int zo) {
	Biome::decorate(level, random, xo, zo);

	if (GENERATE_EMERALD_ORE)
	{
		int emeraldCount = 3 + random->nextInt(6);
		for (int d = 0; d < emeraldCount; d++)
		{
			int x = xo + random->nextInt(16);
			int y = random->nextInt((Level::genDepth / 4) - 4) + 4;
			int z = zo + random->nextInt(16);
			int tile = level->getTile(x, y, z);
			if (tile == Tile::rock_Id)
			{
				level->setTileNoUpdate(x, y, z, Tile::emeraldOre_Id);
			}
		}
	}

}
