#pragma once

#include "Enchantment.h"

class ArrowDamageEnchantment : public Enchantment
{
public:
	ArrowDamageEnchantment(int id, int frequency);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
};