#pragma once

#include "Goal.h"

class PathfinderMob;

class PanicGoal : public Goal
{
private:
	PathfinderMob *mob;
	float speed;
	double posX, posY, posZ;

public:
	PanicGoal(PathfinderMob *mob, float speed);

	virtual bool canUse();
	virtual void start();
	virtual bool canContinueToUse();
};