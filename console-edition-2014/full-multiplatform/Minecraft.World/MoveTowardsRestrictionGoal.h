#pragma once

#include "Goal.h"

class MoveTowardsRestrictionGoal : public Goal
{
private:
	PathfinderMob *mob;
	double wantedX, wantedY, wantedZ;
	float speed;

public:
	MoveTowardsRestrictionGoal(PathfinderMob *mob, float speed);

	bool canUse();
	bool canContinueToUse();
	void start();
};