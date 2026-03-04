#include "stdafx.h"
#include "OxygenEnchantment.h"

OxygenEnchantment::OxygenEnchantment(int id, int frequency) : Enchantment(id, frequency, EnchantmentCategory::armor_head)
{
	setDescriptionId(IDS_ENCHANTMENT_OXYGEN);
}

int OxygenEnchantment::getMinCost(int level)
{
	return 10 * level;
}

int OxygenEnchantment::getMaxCost(int level)
{
	return getMinCost(level) + 30;
}

int OxygenEnchantment::getMaxLevel()
{
	return 3;
}