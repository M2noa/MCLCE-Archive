#include "stdafx.h"
#include "FireAspectEnchantment.h"

FireAspectEnchantment::FireAspectEnchantment(int id, int frequency) : Enchantment(id, frequency, EnchantmentCategory::weapon)
{
	setDescriptionId(IDS_ENCHANTMENT_FIRE);
}

int FireAspectEnchantment::getMinCost(int level)
{
	return 10 + 20 * (level - 1);
}

int FireAspectEnchantment::getMaxCost(int level)
{
	return Enchantment::getMinCost(level) + 50;
}

int FireAspectEnchantment::getMaxLevel()
{
	return 2;
}