#pragma once

#include "Enchantment.h"

class ArrowInfiniteEnchantment : public Enchantment
{
public:
	ArrowInfiniteEnchantment(int id, int frequency);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
};