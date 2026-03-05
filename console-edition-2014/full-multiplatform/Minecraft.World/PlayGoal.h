#pragma once

#include "Goal.h"

class PlayGoal : public Goal
{
private:
	Villager *mob;
	weak_ptr<Mob> followFriend;
	float speed;
	double wantedX, wantedY, wantedZ;
	int playTime;

public:
	PlayGoal(Villager *mob, float speed);

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
	virtual void stop();
	virtual void tick();
};