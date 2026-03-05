#include "stdafx.h"
#include "LightGemTile.h"
#include "net.minecraft.world.item.h"

LightGemTile::LightGemTile(int id, Material *material) : Tile(id, material)
{
}

int LightGemTile::getResourceCountForLootBonus(int bonusLevel, Random *random)
{
	return Mth::clamp(getResourceCount(random) + random->nextInt(bonusLevel + 1), 1, 4);
}

int LightGemTile::getResourceCount(Random *random)
{
	return 2 + random->nextInt(3);
}

int LightGemTile::getResource(int data, Random *random, int playerBonusLevel)
{
	return Item::yellowDust->id;
}