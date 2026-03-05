#pragma once

#include "Goal.h"

class PathfinderMob;

class RandomStrollGoal : public Goal
{
private:
	PathfinderMob *mob;
	double wantedX, wantedY, wantedZ;
	float speed;

public:
	RandomStrollGoal(PathfinderMob *mob, float speed);

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
};