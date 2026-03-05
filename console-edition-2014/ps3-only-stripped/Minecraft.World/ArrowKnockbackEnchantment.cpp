#include "stdafx.h"
#include "ArrowKnockbackEnchantment.h"

ArrowKnockbackEnchantment::ArrowKnockbackEnchantment(int id, int frequency) : Enchantment(id, frequency, EnchantmentCategory::bow)
{
	setDescriptionId(IDS_ENCHANTMENT_ARROW_KNOCKBACK);
}

int ArrowKnockbackEnchantment::getMinCost(int level)
{
	return 12 + (level - 1) * 20;
}

int ArrowKnockbackEnchantment::getMaxCost(int level)
{
	return getMinCost(level) + 25;
}

int ArrowKnockbackEnchantment::getMaxLevel()
{
	return 2;
}