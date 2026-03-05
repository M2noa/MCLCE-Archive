#include "stdafx.h"
#include "TaigaBiome.h"
#include "net.minecraft.world.entity.animal.h"
#include "net.minecraft.world.level.levelgen.feature.h"
#include "net.minecraft.world.level.biome.h"

TaigaBiome::TaigaBiome(int id) : Biome(id)
{
	friendlies_wolf.push_back(new MobSpawnerData(eTYPE_WOLF, 8, 4, 4));	// 4J - moved to their own category

    decorator->treeCount = 10;
    decorator->grassCount = 1;
}

Feature *TaigaBiome::getTreeFeature(Random *random)
{
	if (random->nextInt(3) == 0)
	{
		return new PineFeature();
	}
	return new SpruceFeature(false);
}