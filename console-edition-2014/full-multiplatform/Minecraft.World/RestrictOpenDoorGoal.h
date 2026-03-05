#pragma once

#include "Goal.h"

class PathfinderMob;
class DoorInfo;

class RestrictOpenDoorGoal : public Goal
{
private:
	PathfinderMob *mob;
	weak_ptr<DoorInfo> doorInfo;

public:
	RestrictOpenDoorGoal(PathfinderMob *mob);

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
	virtual void stop();
	virtual void tick();
};