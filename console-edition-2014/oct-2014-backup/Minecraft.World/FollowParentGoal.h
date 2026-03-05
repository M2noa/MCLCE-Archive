#pragma once

#include "Goal.h"

class Animal;

class FollowParentGoal : public Goal
{
private:
	Animal *animal; // Owner of this goal
	weak_ptr<Animal> parent;
	float speed;
	int timeToRecalcPath;

public:
	FollowParentGoal(Animal *animal, float speed);

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
	virtual void stop();
	virtual void tick();
};