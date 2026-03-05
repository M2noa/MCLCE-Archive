#pragma once

#include "Goal.h"
#include "SharedConstants.h"

// note: Mob should implement handleEntityEvent for client state, also ate to take action upon eating
class EatTileGoal : public Goal
{
private:
	static const int EAT_ANIMATION_TICKS = SharedConstants::TICKS_PER_SECOND * 2;

	Mob *mob; // Owner of this goal
	Level *level;
	int eatAnimationTick;

public:
	EatTileGoal(Mob *mob);

	virtual bool canUse();
	virtual void start();
	virtual void stop();
	virtual bool canContinueToUse();
	virtual int getEatAnimationTick();
	virtual void tick();
	
	// 4J Added override to update ai elements when loading entity from schematics
	virtual void setLevel(Level *level) { this->level = level; }
};