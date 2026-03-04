#include "stdafx.h"
#include "LootBonusEnchantment.h"

LootBonusEnchantment::LootBonusEnchantment(int id, int frequency, const EnchantmentCategory *cat) : Enchantment(id, frequency, cat)
{
	setDescriptionId(IDS_ENCHANTMENT_LOOT_BONUS);
	if (cat == EnchantmentCategory::digger)
	{
		setDescriptionId(IDS_ENCHANTMENT_LOOT_BONUS_DIGGER);
	}
}

int LootBonusEnchantment::getMinCost(int level)
{
	return 15 + (level - 1) * 9;
}

int LootBonusEnchantment::getMaxCost(int level)
{
	return Enchantment::getMinCost(level) + 50;
}

int LootBonusEnchantment::getMaxLevel()
{
	return 3;
}

bool LootBonusEnchantment::isCompatibleWith(Enchantment *other) const
{
	return Enchantment::isCompatibleWith(other) && other->id != untouching->id;
}