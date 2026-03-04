#include "stdafx.h"
#include "RainforestBiome.h"
#include "net.minecraft.world.level.levelgen.feature.h"

RainforestBiome::RainforestBiome(int id) : Biome(id)
{
}

Feature *RainforestBiome::getTreeFeature(Random *random)
{
    if (random->nextInt(3) == 0)
	{
        return new BasicTree(false);
    }
    return new TreeFeature(false);
}