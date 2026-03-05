#pragma once

#include "Goal.h"

class ArrowAttackGoal : public Goal
{
public:
	static const int ArrowType = 1;
	static const int SnowballType = 2;

	Level *level;
	Mob *mob; // Owner of this goal
	weak_ptr<Mob> target;
	int attackTime;
	float speed;
	int seeTime;
	int projectileType;
	int attackInterval;

	ArrowAttackGoal(Mob *mob, float speed, int projectileType, int attackInterval);

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void stop();
	virtual void tick();

private:
	void fireAtTarget();

public:
	// 4J Added override to update ai elements when loading entity from schematics
	virtual void setLevel(Level *level) { this->level = level; }
};