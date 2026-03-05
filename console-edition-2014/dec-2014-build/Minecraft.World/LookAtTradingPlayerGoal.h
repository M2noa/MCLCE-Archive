#pragma once

#include "LookAtPlayerGoal.h"

class Villager;

class LookAtTradingPlayerGoal : public LookAtPlayerGoal
{
private:
	Villager *villager; // This is the owner of this goal

public:
	LookAtTradingPlayerGoal(Villager *villager);

	virtual bool canUse();
};