#pragma once

#include "Enchantment.h"

class KnockbackEnchantment : public Enchantment
{
public:
	KnockbackEnchantment(int id, int frequency);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
};