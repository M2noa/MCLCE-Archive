#pragma once

#include "Goal.h"

class TemptGoal : public Goal
{
private:
	PathfinderMob *mob;
	float speed;
	double px, py, pz, pRotX, pRotY;
	weak_ptr<Player> player;
	int calmDown ;
	bool _isRunning;
	int itemId;
	bool canScare;
	bool oldAvoidWater;

public:
	TemptGoal(PathfinderMob *mob, float speed, int itemId, bool canScare);

	bool canUse();
	bool canContinueToUse();
	void start();
	void stop();
	void tick();
	bool isRunning();
};