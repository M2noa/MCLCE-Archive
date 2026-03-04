#pragma once

#include "Goal.h"

class Mob;

class FloatGoal : public Goal
{
private:
	Mob *mob;

public:
	FloatGoal(Mob *mob);

	virtual bool canUse();
	virtual void tick();
};