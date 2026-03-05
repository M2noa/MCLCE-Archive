#include "stdafx.h"
#include "ArrowDamageEnchantment.h"

ArrowDamageEnchantment::ArrowDamageEnchantment(int id, int frequency) : Enchantment(id, frequency, EnchantmentCategory::bow)
{
	setDescriptionId(IDS_ENCHANTMENT_ARROW_DAMAGE);
}

int ArrowDamageEnchantment::getMinCost(int level)
{
	return 1 + (level - 1) * 10;
}

int ArrowDamageEnchantment::getMaxCost(int level)
{
	return getMinCost(level) + 15;
}

int ArrowDamageEnchantment::getMaxLevel()
{
	return 5;
}