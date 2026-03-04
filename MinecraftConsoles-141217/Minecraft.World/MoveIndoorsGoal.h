#pragma once

#include "Goal.h"

class PathfinderMob;
class DoorInfo;

class MoveIndoorsGoal : public Goal
{
private:
	PathfinderMob *mob;
	weak_ptr<DoorInfo> doorInfo;
	int insideX, insideZ;

public:
	MoveIndoorsGoal(PathfinderMob *mob);

	bool canUse();
	bool canContinueToUse();
	void start();
	void stop();
};