#pragma once

#include "Goal.h"

class Villager;

class TradeWithPlayerGoal : public Goal
{
private:
	Villager *mob; // This is the owner of the goal

public:
	TradeWithPlayerGoal(Villager *mob);

	bool canUse();
	void start();
	void stop();
};