#pragma once

#include "Enchantment.h"

class OxygenEnchantment : public Enchantment
{
public:
	OxygenEnchantment(int id, int frequency);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
};