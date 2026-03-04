#pragma once

#include "DoorInteractGoal.h"

class OpenDoorGoal : public DoorInteractGoal
{
private:
	bool closeDoor;
	int forgetTime;

public:
	OpenDoorGoal(Mob *mob, bool closeDoorAfter);

	virtual bool canContinueToUse();
	virtual void start();
	virtual void stop();
	virtual void tick();
};