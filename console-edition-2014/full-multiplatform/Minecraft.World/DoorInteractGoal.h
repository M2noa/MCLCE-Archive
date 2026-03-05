#pragma once

#include "Goal.h"

class DoorTile;

class DoorInteractGoal : public Goal
{
protected:
	Mob *mob; // Owner of this goal
	int doorX, doorY, doorZ;
	DoorTile *doorTile;

private:
	bool passed;
	float doorOpenDirX, doorOpenDirZ;

public:
	DoorInteractGoal(Mob *mob);
	virtual ~DoorInteractGoal() {}

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
	virtual void tick();

private:
	DoorTile *getDoorTile(int x, int y, int z);
};