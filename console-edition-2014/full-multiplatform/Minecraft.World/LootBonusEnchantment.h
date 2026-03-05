#pragma once

#include "Enchantment.h"

class LootBonusEnchantment : public Enchantment
{
public:
	LootBonusEnchantment(int id, int frequency, const EnchantmentCategory *cat);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
	virtual bool isCompatibleWith(Enchantment *other) const;
};