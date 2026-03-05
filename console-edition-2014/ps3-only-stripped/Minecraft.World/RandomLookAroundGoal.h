#pragma once

#include "Goal.h"

class RandomLookAroundGoal : public Goal
{
private:
	Mob *mob;
	double relX, relZ;
	int lookTime;

public:
	RandomLookAroundGoal(Mob *mob);

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
	virtual void tick();
};