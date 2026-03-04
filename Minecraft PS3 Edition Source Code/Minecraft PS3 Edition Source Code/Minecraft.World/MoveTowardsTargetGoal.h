#pragma once

#include "Goal.h"

class MoveTowardsTargetGoal : public Goal
{
private:
	PathfinderMob *mob;
	weak_ptr<Mob> target;
	double wantedX, wantedY, wantedZ;
	float speed, within;

public:
	MoveTowardsTargetGoal(PathfinderMob *mob, float speed, float within);

	bool canUse();
	bool canContinueToUse();
	void stop();
	void start();
};