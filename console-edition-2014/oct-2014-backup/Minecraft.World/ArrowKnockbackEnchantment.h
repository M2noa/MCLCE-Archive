#pragma once

#include "Enchantment.h"

class ArrowKnockbackEnchantment : public Enchantment
{
public:
	ArrowKnockbackEnchantment(int id, int frequency);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
};