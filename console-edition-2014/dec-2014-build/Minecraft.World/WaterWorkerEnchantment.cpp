#include "stdafx.h"
#include "WaterWorkerEnchantment.h"

WaterWorkerEnchantment::WaterWorkerEnchantment(int id, int frequency) : Enchantment(id, frequency, EnchantmentCategory::armor_head)
{
	setDescriptionId(IDS_ENCHANTMENT_WATER_WORKER);
}

int WaterWorkerEnchantment::getMinCost(int level)
{
	return 1;
}

int WaterWorkerEnchantment::getMaxCost(int level)
{
	return getMinCost(level) + 40;
}

int WaterWorkerEnchantment::getMaxLevel()
{
	return 1;
}