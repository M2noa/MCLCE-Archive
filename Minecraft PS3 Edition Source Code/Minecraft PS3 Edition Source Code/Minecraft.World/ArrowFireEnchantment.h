#pragma once

#include "Enchantment.h"

class ArrowFireEnchantment : public Enchantment
{
public:
	ArrowFireEnchantment(int id, int frequency);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
};