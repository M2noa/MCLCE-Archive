#pragma once

#include "Enchantment.h"

class FireAspectEnchantment : public Enchantment
{
public:
	FireAspectEnchantment(int id, int frequency);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
};