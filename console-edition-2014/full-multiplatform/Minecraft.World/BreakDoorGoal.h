#pragma once

#include "DoorInteractGoal.h"
#include "SharedConstants.h"

class BreakDoorGoal : public DoorInteractGoal
{
private:
	static const int DOOR_BREAK_TIME = SharedConstants::TICKS_PER_SECOND * 12;

	int breakTime;
	int lastBreakProgress;

public:
	BreakDoorGoal(Mob *mob);

	virtual bool canUse();
	virtual void start();
	virtual void stop();
	virtual bool canContinueToUse();
	virtual void tick();
};