#include "stdafx.h"
#include "KnockbackEnchantment.h"

KnockbackEnchantment::KnockbackEnchantment(int id, int frequency) : Enchantment(id, frequency, EnchantmentCategory::weapon)
{
	setDescriptionId(IDS_ENCHANTMENT_KNOCKBACK);
}

int KnockbackEnchantment::getMinCost(int level)
{
	return 5 + 20 * (level - 1);
}

int KnockbackEnchantment::getMaxCost(int level)
{
	return Enchantment::getMinCost(level) + 50;
}

int KnockbackEnchantment::getMaxLevel()
{
	return 2;
}