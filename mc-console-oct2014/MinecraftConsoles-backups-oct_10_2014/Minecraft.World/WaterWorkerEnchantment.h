#pragma once

#include "Enchantment.h"

class WaterWorkerEnchantment : public Enchantment
{
public:
	WaterWorkerEnchantment(int id, int frequency);

	virtual int getMinCost(int level);
	virtual int getMaxCost(int level);
	virtual int getMaxLevel();
};