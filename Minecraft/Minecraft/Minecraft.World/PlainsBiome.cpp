#include "stdafx.h"
#include "net.minecraft.world.level.biome.h"

PlainsBiome::PlainsBiome(int id) : Biome(id)
{
	decorator->treeCount = -999;
	decorator->flowerCount = 4;
	decorator->grassCount = 10;
}